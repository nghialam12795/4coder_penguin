/**
A file written by:
 _______          .__    .__         .____
 \      \    ____ |  |__ |__|____    |    |   _____    _____
 /   |   \  / ___\|  |  \|  \__  \   |    |   \__  \  /     \
/    |    \/ /_/  >   Y  \  |/ __ \_ |    |___ / __ \|  Y Y  \
\____|__  /\___  /|___|  /__(____  / |_______ (____  /__|_|  /
        \//_____/      \/        \/          \/    \/      \/
--------------------------------------------------------------
License: GNU GENERAL PUBLIC LICENSE Version 3

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef FCODER_PENGUIN_
#define FCODER_PENGUIN_

#include "4coder_default_include.cpp"

#include "4coder_penguin_utils.cpp"
#include "4coder_penguin_dashboard.cpp"

#define SNIPPET_EXPANSION "4coder_penguin/snippets/snippets.inc"
global i32 global_snippet_count = 0;
#include "4coder_penguin/4coder_penguin_vi_bindings.cpp"

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

//~ TODO(nghialam)
//
// [x] Vi/Vim modal key-binding
// [x] Auto Snippets
// [ ] Setup working workflow with 4coder and CMake projects


//~ NOTE(nghialam): Custom Layer Initilization
void custom_layer_init(Application_Links* app) {
    Thread_Context* tctx = get_thread_context(app);
    
    // Base 4coder Initialization
    default_framework_init(app);
    set_all_default_hooks(app);
    mapping_init(tctx, &framework_mapping);
#if OS_MAC
    setup_mac_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
#else
    setup_default_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
#endif
    
    // Penguin Vim Custom Layer
    global_snippet_count = ArrayCount(default_snippets);
    vim_init(app);
    
    vim_set_default_hooks(app);
    Vim_Key vim_leader_key = vim_key(KeyCode_Space); // Or whatever you prefer
    vim_setup_default_mapping(app, &framework_mapping, vim_leader_key);
}

//~ NOTE: Startup.

// TODO: This is only being used to check if a font file exists because
// there's a bug in try_create_new_face that crashes the program if a font is
// not found. This function is only necessary until that is fixed.
static b32 IsFileReadable(String_Const_u8 path) {
    b32 result = 0;
    FILE *file = fopen((char *)path.str, "r");
    if(file) {
        result = 1;
        fclose(file);
    }
    return result;
}

CUSTOM_COMMAND_SIG(penguin_startup)
CUSTOM_DOC("Penguin startup event") {
    ProfileScope(app, "default startup");
    User_Input input = get_current_input(app);
    if(match_core_code(&input, CoreCode_Startup)) {
        String_Const_u8_Array file_names = input.event.core.file_names;
        load_themes_default_folder(app);
        default_4coder_initialize(app, file_names);
        default_4coder_side_by_side_panels(app, file_names);

        if (global_config.automatically_load_project) {
            load_project(app);
        }

        // NOTE: Open Dashboard
        // dashboard_open(app);
        
        // NOTE(rjf): Initialize stylish fonts.
        {
            Scratch_Block scratch(app);
            String_Const_u8 bin_path = system_get_path(scratch, SystemPath_Binary);
            
            // NOTE(rjf): Title font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/JetBrainsMono-Bold-Italic.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 18;
                    desc.parameters.bold = 0;
                    desc.parameters.italic = 0;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name)) {
                    global_styled_title_face = try_create_new_face(app, &desc);
                } 
            }
            
            // NOTE(rjf): Label font.
            {
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/JetBrainsMono-Medium-Italic.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = 10;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name)) {
                    global_styled_label_face = try_create_new_face(app, &desc);
                }  
            }
            
            // NOTE(rjf): Small code font.
            {
                Face_Description normal_code_desc = get_face_description(app, get_face_id(app, 0));
                
                Face_Description desc = {0};
                {
                    desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/JetBrainsMono-Regular.ttf", string_expand(bin_path));
                    desc.parameters.pt_size = normal_code_desc.parameters.pt_size - 1;
                    desc.parameters.bold = 1;
                    desc.parameters.italic = 1;
                    desc.parameters.hinting = 0;
                }
                
                if(IsFileReadable(desc.font.file_name)) {
                    global_small_code_face = try_create_new_face(app, &desc);
                } 
            }
        }
    }
}

#endif // FCODER_PENGUIN_  