export namespace fast_io
{

    namespace win32::nt
    {
        using ::fast_io::win32::nt::nt_get_current_peb;
        using ::fast_io::win32::nt::CsrClientCallServer;
        using ::fast_io::win32::nt::RtlQueryEnvironmentVariable_U;
        using ::fast_io::win32::nt::unicode_string;
        using ::fast_io::win32::nt::nt_set_information_virtual_memory;
    } // namespace win32::nt
}