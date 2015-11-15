#include "test_support/file_support.h"
#include "test_support/catch.hh"
#include "util/format.h"
#include "util/pack/zlib.h"
#include "util/range.h"

using namespace au;

std::shared_ptr<File> tests::stub_file(
    const std::string &name, const bstr &data)
{
    return std::make_shared<File>(name, data);
}

std::shared_ptr<File> tests::file_from_path(
    const boost::filesystem::path &path, const std::string &cust_name)
{
    auto ret = std::make_shared<File>(path, io::FileMode::Read);
    if (!cust_name.empty())
        ret->name = cust_name;
    return ret;
}

std::shared_ptr<File> tests::zlib_file_from_path(
    const boost::filesystem::path &path, const std::string &cust_name)
{
    File compressed_file(path, io::FileMode::Read);
    const auto compressed_data = compressed_file.io.read_to_eof();
    const auto decompressed_data = util::pack::zlib_inflate(compressed_data);
    return std::make_shared<File>(
        cust_name.empty() ? compressed_file.name : cust_name,
        decompressed_data);
}

void tests::compare_files(
    const std::vector<std::shared_ptr<File>> &expected_files,
    const std::vector<std::shared_ptr<File>> &actual_files,
    const bool compare_file_names)
{
    REQUIRE(actual_files.size() == expected_files.size());
    for (auto i : util::range(expected_files.size()))
    {
        const auto &expected_file = expected_files[i];
        const auto &actual_file = actual_files[i];
        INFO(util::format("Files at index %d differ", i));
        tests::compare_files(*expected_file, *actual_file, compare_file_names);
    }
}

void tests::compare_files(
    const File &expected_file,
    const File &actual_file,
    const bool compare_file_names)
{
    if (compare_file_names)
        REQUIRE(expected_file.name == actual_file.name);
    REQUIRE(expected_file.io.size() == actual_file.io.size());
    expected_file.io.seek(0);
    actual_file.io.seek(0);
    const auto expected_content = expected_file.io.read_to_eof();
    const auto actual_content = actual_file.io.read_to_eof();
    INFO("Expected content: " << (expected_content.size() < 1000
        ? expected_content.str()
        : "(too big to display)"));
    INFO("Actual content: " << (actual_content.size() < 1000
        ? actual_content.str()
        : "(too big to display)"));
    REQUIRE(expected_content == actual_content);
}
