#include "file_io.h"
#include "formats/french_bread/ex3_converter.h"
#include "util/zlib.h"
#include "test_support/eassert.h"
using namespace Formats::FrenchBread;

void test_ex3_decoding()
{
    Ex3Converter converter;
    const std::string input_path
        = "tests/formats/french_bread/files/WIN_HISUI&KOHAKU.EX3";
    const std::string expected_path
        = "tests/formats/french_bread/files/WIN_HISUI&KOHAKU-out.bmpz";

    FileIO input_io(input_path, FileIOMode::Read);
    File file;
    file.io.write_from_io(input_io, input_io.size());
    converter.decode(file);

    FileIO expected_io(expected_path, FileIOMode::Read);
    const std::string expected_data
        = zlib_inflate(expected_io.read(expected_io.size()));

    file.io.seek(0);
    eassert(expected_data.size() == file.io.size());
    eassert(expected_data == file.io.read(file.io.size()));
}

int main(void)
{
    test_ex3_decoding();
    return 0;
}