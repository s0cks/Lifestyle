#include <hoedown/document.h>
#include <hoedown/escape.h>
#include <string.h>

#define HEADER_LEN 100

static void
lifestyle_buffer_putrep(hoedown_buffer* ob, char c, int times){
    for(int i = 0; i < times; i++){
        hoedown_buffer_putc(ob, c);
    }
}

static void
rndr_gen_header(hoedown_buffer *ob, int inline_render, const hoedown_renderer_data *data){
    hoedown_buffer_puts(ob, "/*\n *");
    lifestyle_buffer_putrep(ob, '=', HEADER_LEN);
    hoedown_buffer_putc(ob, '\n');
}

static void
rndr_header(hoedown_buffer* ob, const hoedown_buffer* content, int level, const hoedown_renderer_data* data){
    if(content){
        hoedown_buffer_put(ob, content->data, content->size);
    }
}

static void
rndr_normal_text(hoedown_buffer* ob, const hoedown_buffer* content, const hoedown_renderer_data* data){
    if(content){
        hoedown_buffer_puts(ob, " *");
        lifestyle_buffer_putrep(ob, ' ', 3);

        int len = 3;
        for(int i = 0; i < content->size; i++){
            hoedown_buffer_putc(ob, content->data[i]);
            len++;
            if(len >= (HEADER_LEN - 3)){
                hoedown_buffer_putc(ob, '\n');
                hoedown_buffer_puts(ob, " *");
                hoedown_buffer_putc(ob, ' ');
                len = 1;
            }
        }

        lifestyle_buffer_putrep(ob, ' ', 3);
        hoedown_buffer_puts(ob, "\n *\n");
    }
}

static void
rndr_hrule(hoedown_buffer *ob, const hoedown_renderer_data *data){
    hoedown_buffer_puts(ob, " *");
    lifestyle_buffer_putrep(ob, '-', HEADER_LEN);
    hoedown_buffer_puts(ob, "\n");
}

static void
rndr_pg(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data){
    hoedown_buffer_put(ob, content->data, content->size);
}

int list_count = 0;

static void
rndr_list(hoedown_buffer* ob, const hoedown_buffer* content, hoedown_list_flags flags, const hoedown_renderer_data* data){
    list_count = 0;
    if(content){
        hoedown_buffer_put(ob, content->data, content->size);
    }
    hoedown_buffer_puts(ob, " *\n");
}


static void
rndr_list_item(hoedown_buffer *ob, const hoedown_buffer *content, hoedown_list_flags flags, const hoedown_renderer_data *data){
    hoedown_buffer_puts(ob, " *");
    lifestyle_buffer_putrep(ob, ' ', 4);
    if(flags & HOEDOWN_LIST_ORDERED){
        hoedown_buffer_printf(ob, "%d.) ", (1 + list_count++));

        if(content){
            size_t size = content->size;
            hoedown_buffer_put(ob, content->data, size);
        } else{
            hoedown_buffer_puts(ob, "[error]");
        }

        hoedown_buffer_putc(ob, '\n');
    } else{
        hoedown_buffer_putc(ob, '+');

        if(content){
            size_t size = content->size;
            hoedown_buffer_put(ob, content->data, size);
        } else{
            hoedown_buffer_puts(ob, "[error]");
        }

        hoedown_buffer_putc(ob, '\n');
    }
}

static int
rndr_link(hoedown_buffer* ob, const hoedown_buffer* content, const hoedown_buffer* link, const hoedown_buffer* title, const hoedown_renderer_data* data){
    hoedown_buffer_puts(ob, " *");
    lifestyle_buffer_putrep(ob, ' ', 4);

    if(title && title->size){
        hoedown_escape_html(ob, title->data, title->size, 0);
    }

    hoedown_buffer_putc(ob, ' ');
    hoedown_buffer_puts(ob, "=>");
    hoedown_buffer_putc(ob, ' ');

    if(link && link->size){
        hoedown_escape_href(ob, link->data, link->size);
    }

    hoedown_buffer_puts(ob, "\n *\n *\n");
    return 1;
}

static void
rndr_table(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data){
    hoedown_buffer_put(ob, content->data, content->size);
}

static void
rndr_table_header(hoedown_buffer *ob, const hoedown_buffer *content, const hoedown_renderer_data *data){
    lifestyle_buffer_putrep(ob, ' ', 4);
    lifestyle_buffer_putrep(ob, '-', (int) content->size + 6);
    hoedown_buffer_puts(ob, "\n*    |");
    hoedown_buffer_put(ob, content->data, content->size);
    lifestyle_buffer_putrep(ob, ' ', 4);
    hoedown_buffer_putc(ob, '|');
    lifestyle_buffer_putrep(ob, '-', (int) content->size + 6);
}

static void
rndr_gen_footer(hoedown_buffer *ob, int inline_render, const hoedown_renderer_data *data){
    hoedown_buffer_puts(ob, " *");
    lifestyle_buffer_putrep(ob, '=', HEADER_LEN);
    hoedown_buffer_puts(ob, "\n */");
}

int main(int argc, char** argv){
    fprintf(stdout, "Parsing: %s.\n", argv[1]);
    hoedown_document* doc;

    static const hoedown_renderer cb_default ={
            NULL, // ?
            NULL, // Block Code
            NULL, // Block Quote
            &rndr_header, // Header
            &rndr_hrule, // H Rule
            &rndr_list, // List
            &rndr_list_item, // List Item
            &rndr_pg, // Paragraph
            &rndr_table, // Table
            &rndr_table_header, // Table Header
            NULL, // Table Body
            NULL, // Table Row
            NULL, // Table Cell
            NULL, // Foot Notes
            NULL, // Foot Notes Def
            NULL, // Raw Block
            NULL, // Autolink
            NULL, // Codespan
            NULL, // Double Emphasis
            NULL, // Emphasis
            NULL, // Underline
            NULL, // Highlight
            NULL, // Quote
            NULL, // Image
            NULL, // Linebreak
            &rndr_link, // Link
            NULL, // Triple Emphasis
            NULL, // Strikethrough
            NULL, // Superscript
            NULL, // Foot Note Ref
            NULL, // Math
            NULL, // Raw HTML
            NULL, // ?
            &rndr_normal_text, // Normal Text
            &rndr_gen_header, // Header
            &rndr_gen_footer, // Footer
    };

    hoedown_renderer* renderer = (hoedown_renderer *) hoedown_malloc(sizeof(hoedown_renderer));
    memcpy(renderer, &cb_default, sizeof(hoedown_renderer));
    hoedown_buffer* ib = hoedown_buffer_new(1024);
    FILE* ifile = fopen(argv[1], "r");
    if(hoedown_buffer_putf(ib, ifile)){
        fprintf(stderr, "I/O errors found while reading input.\n");
        return 5;
    }

    if(ifile != stdin){
        fclose(ifile);
    }

    hoedown_buffer* ob = hoedown_buffer_new(1024);
    doc = hoedown_document_new(renderer, (hoedown_extensions) 0, 1);
    hoedown_document_render(doc, ob, ib->data, ib->size);
    hoedown_buffer_free(ib);
    hoedown_document_free(doc);
    fprintf(stdout, "Output:\n");
    fprintf(stdout, "%s\n", ob->data);
    hoedown_buffer_free(ob);

    return 0;
}