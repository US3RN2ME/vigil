#include "EtwSession.hpp"

#include "WinApi.hpp"
#include "vigil/SystemError.hpp"

#include <vigil/Logger.hpp>

namespace vigil::platform {
   namespace {

      // EVENT_TRACE_PROPERTIES must be followed by the session name in the same
      // allocation. We embed a fixed-size name buffer directly after the struct.
      struct PropertiesBuffer {
         EVENT_TRACE_PROPERTIES props;
         wchar_t name[256];
      };

      PropertiesBuffer makeProperties(const std::wstring& name) {
         PropertiesBuffer buf{};
         buf.props.Wnode.BufferSize = sizeof(buf);
         buf.props.Wnode.Flags = WNODE_FLAG_TRACED_GUID;
         buf.props.Wnode.ClientContext = 1; // QPC clock resolution
         buf.props.LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
         buf.props.LoggerNameOffset = offsetof(PropertiesBuffer, name);
         wcsncpy_s(buf.name, name.c_str(), _TRUNCATE);
         return buf;
      }

      GUID toGuid(const std::array<uint8_t, 16>& bytes) {
         GUID g{};
         static_assert(sizeof(g) == 16);
         memcpy(&g, bytes.data(), 16);
         return g;
      }

      // File-scope callback — WINAPI calling convention required by ETW.
      // Forwards to the EtwSession instance stored in EVENT_RECORD.UserContext.
      void WINAPI etwDispatch(EVENT_RECORD* record) {
         static_cast<EtwSession*>(record->UserContext)->notify(*record);
      }

   } // namespace

   std::optional<EtwSession> EtwSession::start(std::wstring sessionName, std::array<uint8_t, 16> providerGuid,
                                               uint64_t keyword) {
      EtwSession session;
      session.name_ = std::move(sessionName);
      session.providerGuid_ = providerGuid;

      // Stop any leftover session with the same name before starting fresh.
      {
         auto staleProps = makeProperties(session.name_);
         ControlTraceW(0, session.name_.c_str(), &staleProps.props, EVENT_TRACE_CONTROL_STOP);
      }

      TRACEHANDLE sessionHandle{};
      auto props = makeProperties(session.name_);
      const ULONG startErr = StartTraceW(&sessionHandle, session.name_.c_str(), &props.props);
      if (startErr != ERROR_SUCCESS) {
         log::error("ETW StartTrace failed: '{}'", error::message(startErr));
         return {};
      }
      session.sessionHandle_ = static_cast<uint64_t>(sessionHandle);

      ENABLE_TRACE_PARAMETERS params{};
      params.Version = ENABLE_TRACE_PARAMETERS_VERSION_2;

      const GUID guid = toGuid(providerGuid);
      const ULONG enableErr = EnableTraceEx2(sessionHandle, &guid, EVENT_CONTROL_CODE_ENABLE_PROVIDER, TRACE_LEVEL_INFORMATION,
                                             keyword, 0, 0, &params);

      if (enableErr != ERROR_SUCCESS) {
         log::error("ETW EnableTraceEx2 failed: '{}'", error::message(enableErr));
         auto stopProps = makeProperties(session.name_);
         ControlTraceW(sessionHandle, nullptr, &stopProps.props, EVENT_TRACE_CONTROL_STOP);
         session.sessionHandle_ = kInvalid;
         return {};
      }

      return session;
   }

   EtwSession::~EtwSession() {
      stop();
   }

   EtwSession::EtwSession(EtwSession&& other) noexcept
       : name_{std::move(other.name_)}
       , providerGuid_{other.providerGuid_}
       , sessionHandle_{other.sessionHandle_}
       , consumerHandle_{other.consumerHandle_}
       , callback_{std::move(other.callback_)} {
      other.sessionHandle_ = kInvalid;
      other.consumerHandle_ = kInvalid;
   }

   EtwSession& EtwSession::operator=(EtwSession&& other) noexcept {
      if (this != &other) {
         stop();
         name_ = std::move(other.name_);
         providerGuid_ = other.providerGuid_;
         sessionHandle_ = other.sessionHandle_;
         consumerHandle_ = other.consumerHandle_;
         callback_ = std::move(other.callback_);
         other.sessionHandle_ = kInvalid;
         other.consumerHandle_ = kInvalid;
      }
      return *this;
   }

   void EtwSession::notify(const _EVENT_RECORD& event) {
      if (callback_)
         callback_(event);
   }

   void EtwSession::consume(EventCallback callback) {
      callback_ = std::move(callback);

      EVENT_TRACE_LOGFILEW logfile{};
      logfile.LoggerName = const_cast<wchar_t*>(name_.c_str());
      logfile.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
      logfile.EventRecordCallback = etwDispatch;
      logfile.Context = this; // forwarded to EVENT_RECORD.UserContext

      PROCESSTRACE_HANDLE consumerHandle = OpenTraceW(&logfile);
      if (consumerHandle == static_cast<TRACEHANDLE>(kInvalid)) {
         log::error("ETW OpenTrace failed: '{}'", error::lastMessage());
         return;
      }
      consumerHandle_ = static_cast<uint64_t>(consumerHandle);

      ProcessTrace(&consumerHandle, 1, nullptr, nullptr);
   }

   void EtwSession::stop() {
      if (consumerHandle_ != kInvalid) {
         CloseTrace(static_cast<TRACEHANDLE>(consumerHandle_)); // unblocks ProcessTrace
         consumerHandle_ = kInvalid;
      }
      if (sessionHandle_ != kInvalid) {
         auto props = makeProperties(name_);
         ControlTraceW(static_cast<TRACEHANDLE>(sessionHandle_), nullptr, &props.props, EVENT_TRACE_CONTROL_STOP);
      }
   }
} // namespace vigil::platform
