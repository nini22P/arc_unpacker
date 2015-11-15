#include "fmt/fvp/bin_archive_decoder.h"
#include "test_support/catch.hh"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"

using namespace au;
using namespace au::fmt::fvp;

static const std::string dir = "tests/fmt/fvp/files/bin/";

static void do_test(const std::string &input_path)
{
    const std::vector<std::shared_ptr<File>> expected_files
    {
        tests::stub_file("123.txt", "1234567890"_b),
        tests::stub_file("abc.xyz", "abcdefghijklmnopqrstuvwxyz"_b),
    };
    const BinArchiveDecoder decoder;
    const auto input_file = tests::file_from_path(dir + input_path);
    const auto actual_files = tests::unpack(decoder, *input_file);
    tests::compare_files(expected_files, actual_files, true);
}

TEST_CASE("FVP BIN archives", "[fmt]")
{
    do_test("test.bin");
}
