#include "fmt/glib/glib2_archive_decoder.h"
#include "test_support/catch.hh"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"

using namespace au;
using namespace au::fmt::glib;

static const std::string dir = "tests/fmt/glib/files/glib2/";

static void do_test(
    const std::string &input_path,
    const std::vector<std::shared_ptr<File>> &expected_files,
    const bool input_file_is_compressed = false)
{
    const Glib2ArchiveDecoder decoder;
    const auto input_file = input_file_is_compressed
        ? tests::zlib_file_from_path(dir + input_path)
        : tests::file_from_path(dir + input_path);
    const auto actual_files = tests::unpack(decoder, *input_file);
    tests::compare_files(expected_files, actual_files, true);
}

TEST_CASE("GLib GLib2 archives", "[fmt]")
{
    SECTION("Musume Shimai plugin")
    {
        do_test(
            "musume.g2",
            {
                tests::stub_file("123.txt", "1234567890"_b),
                tests::stub_file("abc.txt", "abcdefghijklmnopqrstuvwxyz"_b),
            });
    }

    SECTION("Mei Shoujo plugin")
    {
        do_test(
            "mei.g2",
            {
                tests::stub_file("123.txt", "1234567890"_b),
                tests::stub_file("abc.txt", "abcdefghijklmnopqrstuvwxyz"_b),
            });
    }

    SECTION("Nested directories")
    {
        do_test(
            "nest.g2",
            {
                tests::stub_file("test/nest/123.txt", "1234567890"_b),
                tests::stub_file(
                    "test/abc.txt", "abcdefghijklmnopqrstuvwxyz"_b),
            });
    }

    SECTION("Multiple decryption passes")
    {
        do_test(
            "big-zlib.g2",
            {
                tests::stub_file("big.txt", bstr(0xA0000, 0xFF)),
            },
            true);
    }
}
