export namespace fast_io
{

#if defined(_WIN32) || defined(__CYGWIN__)
using ::fast_io::win32_family;
using ::fast_io::nt_family;

using ::fast_io::basic_nt_family_io_observer;
using ::fast_io::basic_nt_family_file;
using ::fast_io::nt_io_observer;
using ::fast_io::nt_file;

using ::fast_io::nt_stdin;
using ::fast_io::nt_stdout;
using ::fast_io::nt_stderr;

using ::fast_io::basic_win32_family_io_observer;
using ::fast_io::basic_win32_family_file;
using ::fast_io::win32_io_observer;
using ::fast_io::win32_file;

using ::fast_io::win32_stdin;
using ::fast_io::win32_stdout;
using ::fast_io::win32_stderr;

#endif

using ::fast_io::basic_posix_family_io_observer;
using ::fast_io::basic_posix_family_file;
using ::fast_io::posix_io_observer;
using ::fast_io::posix_file;
using ::fast_io::native_file;
using ::fast_io::native_io_observer;

using ::fast_io::at;
using ::fast_io::drt;
using ::fast_io::current;
using ::fast_io::recursive;

using ::fast_io::native_file_loader;

using ::fast_io::in;
using ::fast_io::out;
using ::fast_io::err;
using ::fast_io::u8in;
using ::fast_io::u8out;
using ::fast_io::u8err;

using ::fast_io::posix_stdin;
using ::fast_io::posix_stdout;
using ::fast_io::posix_stderr;

} // namespace fast_io
