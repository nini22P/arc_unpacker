#include "fmt/truevision/tga_image_decoder.h"
#include "test_support/catch.hh"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"
#include "test_support/image_support.h"

using namespace au;
using namespace au::fmt::truevision;

static const std::string dir = "tests/fmt/truevision/files/tga/";

static void do_test(
    const std::string &input_path, const std::string &expected_path)
{
    const TgaImageDecoder decoder;
    const auto input_file = tests::file_from_path(dir + input_path);
    const auto expected_file = tests::image_from_path(dir + expected_path);
    const auto actual_file = tests::decode(decoder, *input_file);
    tests::compare_images(*expected_file, actual_file);
}

TEST_CASE("Truevision TGA images", "[fmt]")
{
    SECTION("8-bit, compressed")
    {
        do_test("compressed-8-bit.tga", "compressed-8-bit-out.png");
    }

    SECTION("16-bit, compressed")
    {
        do_test("compressed-16-bit.tga", "compressed-16-bit-out.png");
    }

    SECTION("24-bit, compressed")
    {
        do_test("compressed-24-bit.tga", "compressed-24-bit-out.png");
    }

    SECTION("32-bit, compressed")
    {
        do_test("compressed-32-bit.tga", "compressed-32-bit-out.png");
    }

    SECTION("8-bit, uncompressed, palette")
    {
        do_test(
            "uncompressed-8-bit-palette.tga",
            "uncompressed-8-bit-palette-out.png");
    }

    SECTION("16-bit, uncompressed")
    {
        do_test("uncompressed-16-bit.tga", "uncompressed-16-bit-out.png");
    }

    SECTION("24-bit, uncompressed")
    {
        do_test("uncompressed-24-bit.tga", "uncompressed-24-bit-out.png");
    }

    SECTION("32-bit, uncompressed")
    {
        do_test("uncompressed-32-bit.tga", "uncompressed-32-bit-out.png");
    }

    SECTION("16-bit, uncompressed, flipped")
    {
        do_test(
            "uncompressed-16-bit-flipped.tga",
            "uncompressed-16-bit-flipped-out.png");
    }
}
