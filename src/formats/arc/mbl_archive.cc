// MBL archive
//
// Company:   Ivory
// Engine:    MarbleEngine
// Extension: .mbl
//
// Known games:
// - Wanko to Kurasou

#include "formats/arc/mbl_archive.h"
#include "formats/gfx/prs_converter.h"
#include "string_ex.h"

namespace
{
    typedef struct UnpackContext
    {
        IO &arc_io;
        PrsConverter &prs_converter;
        size_t name_length;

        UnpackContext(
            IO &arc_io, PrsConverter &prs_converter, size_t name_length)
            : arc_io(arc_io),
                prs_converter(prs_converter),
                name_length(name_length)
        {
        }
    } UnpackContext;

    int check_version(
        IO &arc_io,
        size_t initial_position,
        uint32_t file_count,
        uint32_t name_length)
    {
        arc_io.seek(initial_position + file_count * (name_length + 8));
        arc_io.skip(-8);
        uint32_t last_file_offset = arc_io.read_u32_le();
        uint32_t last_file_size = arc_io.read_u32_le();
        return last_file_offset + last_file_size == arc_io.size();
    }

    int get_version(IO &arc_io)
    {
        uint32_t file_count = arc_io.read_u32_le();
        if (check_version(arc_io, 4, file_count, 16))
        {
            arc_io.seek(0);
            return 1;
        }

        arc_io.seek(4);
        uint32_t name_length = arc_io.read_u32_le();
        if (check_version(arc_io, 8, file_count, name_length))
        {
            arc_io.seek(0);
            return 2;
        }

        return -1;
    }

    std::unique_ptr<VirtualFile> read_file(void *context)
    {
        UnpackContext *unpack_context = (UnpackContext*)context;
        std::unique_ptr<VirtualFile> file(new VirtualFile);

        size_t old_pos = unpack_context->arc_io.tell();
        std::string name = unpack_context->arc_io.read_until_zero();
        file->name = convert_encoding(name, "sjis", "utf-8");
        unpack_context->arc_io.seek(old_pos + unpack_context->name_length);

        size_t offset = unpack_context->arc_io.read_u32_le();
        size_t size = unpack_context->arc_io.read_u32_le();
        if (offset + size > unpack_context->arc_io.size())
            throw std::runtime_error("Bad offset to file");

        old_pos = unpack_context->arc_io.tell();
        unpack_context->arc_io.seek(offset);
        file->io.write_from_io(unpack_context->arc_io, size);
        unpack_context->arc_io.seek(old_pos);

        unpack_context->prs_converter.try_decode(*file);
        return file;
    }
}

struct MblArchive::Context
{
    PrsConverter *prs_converter;
};

MblArchive::MblArchive()
{
    context = new MblArchive::Context();
    context->prs_converter = new PrsConverter();
}

MblArchive::~MblArchive()
{
    delete context->prs_converter;
    delete context;
}

void MblArchive::add_cli_help(ArgParser &arg_parser) const
{
    context->prs_converter->add_cli_help(arg_parser);
}

void MblArchive::parse_cli_options(ArgParser &arg_parser)
{
    context->prs_converter->parse_cli_options(arg_parser);
}

void MblArchive::unpack_internal(
    VirtualFile &file, OutputFiles &output_files) const
{
    size_t i;
    int version = get_version(file.io);
    if (version == -1)
        throw std::runtime_error("Not a MBL archive");

    uint32_t file_count = file.io.read_u32_le();
    uint32_t name_length = version == 2 ? file.io.read_u32_le() : 16;
    UnpackContext unpack_context(
        file.io, *context->prs_converter, name_length);

    for (i = 0; i < file_count; i ++)
        output_files.save(&read_file, &unpack_context);
}