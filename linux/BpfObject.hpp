
#ifndef VIGIL_LINUX_BPFOBJECT_HPP
#define VIGIL_LINUX_BPFOBJECT_HPP

#include <string_view>

struct bpf_object;

namespace vigil::linux {
    class BpfObject {
    public:
        explicit BpfObject(std::string_view path);

        ~BpfObject();

        BpfObject(const BpfObject &) = delete;

        BpfObject &operator=(const BpfObject &) = delete;

        BpfObject(BpfObject &&o) noexcept;

        BpfObject &operator=(BpfObject &&o) noexcept;

        void load();

        void attach(std::string_view programName);

        [[nodiscard]] int mapFd(std::string_view mapName) const;

    private:
        struct bpf_object *obj_{};
    };
} // namespace vigil::linux

#endif // VIGIL_LINUX_BPFOBJECT_HPP
