// ==========================================================================
// A file written by:
//  _______          .__    .__         .____
//  \      \    ____ |  |__ |__|____    |    |   _____    _____
//  /   |   \  / ___\|  |  \|  \__  \   |    |   \__  \  /     \
// /    |    \/ /_/  >   Y  \  |/ __ \_ |    |___ / __ \|  Y Y  \
// \____|__  /\___  /|___|  /__(____  / |_______ (____  /__|_|  /
//    /_____/     \/      \/        \/                \/      \/
// --------------------------------------------------------------
// License: GNU GENERAL PUBLIC LICENSE Version 3
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// ==========================================================================

//~ NOTE(Nghia Lam): For DION team docs server stuff.
#if OS_WINDOWS
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
typedef int socklen_t;
#pragma comment(lib, "Ws2_32.lib")
#endif

//~ NOTE(Nghia Lam): Macros and pragmase stuff that have to be put here for various
// reasons
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "4coder_default_include.cpp"
#pragma warning(disable : 4706)
#pragma warning(disable : 4456)
#define COMMAND_SERVER_PORT 4041
#define COMMAND_SERVER_UPDATE_PERIOD_MS 200
#define COMMAND_SERVER_AUTO_LAUNCH_IF_FILE_PRESENT "project_namespaces.txt"

//~ NOTE(Nghia Lam): @f4_headers
#include "4coder_nghialam/external/fleury/4coder_fleury_ubiquitous.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_audio.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_lang.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_index.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_colors.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_render_helpers.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_brace.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_error_annotations.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_divider_comments.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_power_mode.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_cursor.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_plot.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_calc.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_lego.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_pos_context_tooltips.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_code_peek.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_recent_files.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_bindings.h"
#include "4coder_nghialam/external/fleury/4coder_fleury_base_commands.h"
#if OS_WINDOWS
#include "4coder_nghialam/external/fleury/4coder_fleury_command_server.h"
#endif
#include "4coder_nghialam/external/fleury/4coder_fleury_hooks.h"

//~ NOTE(Nghia Lam): @f4_src
#include "4coder_nghialam/external/fleury/4coder_fleury_ubiquitous.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_audio.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_lang.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_index.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_colors.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_render_helpers.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_brace.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_error_annotations.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_divider_comments.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_power_mode.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_cursor.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_plot.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_calc.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_lego.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_pos_context_tooltips.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_code_peek.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_recent_files.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_bindings.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_base_commands.cpp"
#if OS_WINDOWS
#include "4coder_nghialam/external/fleury/4coder_fleury_command_server.cpp"
#endif
#include "4coder_nghialam/external/fleury/4coder_fleury_casey.cpp"
#include "4coder_nghialam/external/fleury/4coder_fleury_hooks.cpp"

//~ NOTE(Nghia Lam): Plots Demo File
#include "external/fleury/4coder_fleury_plots_demo.cpp"

// NOTE(Nghia Lam): My own configuration
#include "4coder_nghialam/4coder_nghialam_base.cpp"
#include "4coder_nghialam/4coder_nghialam_helpers.cpp"
#include "4coder_nghialam/4coder_nghialam_vim.cpp"
#include "4coder_nghialam/4coder_nghialam_draw.cpp"
#include "4coder_nghialam/4coder_nghialam_hook.cpp"
#include "4coder_nghialam/4coder_nghialam_commands.cpp"
#include "4coder_nghialam/4coder_nghialam_bindings.cpp"

//~ NOTE(Nghia Lam): 4coder Stuff. Remember, only include use this one once.
#include "generated/managed_id_metadata.cpp"

//~ TODO(Nghia Lam): Create my custom layer
//  - [x] VI Model Editing System ?? -> Do we need any more operators ?
//  - [x] Stuff for rendering
//  - [ ] My own file bar rendering
//  - [ ] Project management
//  - [ ] Project todo list

//~ NOTE(Nghia Lam): Entry Point here
void custom_layer_init(Application_Links *app) {
  // NOTE(Allen): Default Setups
  default_framework_init(app);
  global_frame_arena = make_arena(get_base_allocator_system());
  permanent_arena = make_arena(get_base_allocator_system());
  set_all_default_hooks(app);
  
  // NOTE(Nghia Lam): Custom Layer Setups
  NL_SetupCustomHooks(app);
  
  // Set up mapping.
  Thread_Context *tctx = get_thread_context(app);
  mapping_init(tctx, &framework_mapping);
  
  String_ID global_map_id = vars_save_string_lit("keys_global");
  String_ID file_map_id   = vars_save_string_lit("keys_file");
  String_ID code_map_id   = vars_save_string_lit("keys_code");
  
  NL_SetupNecessaryBindings(&framework_mapping);
  NL_SetupDefaultBindings(&framework_mapping, global_map_id, file_map_id, code_map_id);
  NL_SetupNecessaryBindings(&framework_mapping);
  NL_SetupVimBindings(&framework_mapping, global_map_id);
  
  // NOTE(rjf): Set up custom code index.
  {
    F4_Index_Initialize();
  }
  
  // NOTE(rjf): Register languages.
  {
    F4_RegisterLanguages();
  }
}

//~ NOTE(rjf): @f4_startup Whenever 4coder's core is ready for the custom layer to start up,
// this is called.

// TODO(rjf): This is only being used to check if a font file exists because
// there's a bug in try_create_new_face that crashes the program if a font is
// not found. This function is only necessary until that is fixed.
function b32 IsFileReadable(String_Const_u8 path) {
  b32 result = 0;
  FILE *file = fopen((char *)path.str, "r");
  if (file) {
    result = 1;
    fclose(file);
  }
  return result;
}

CUSTOM_COMMAND_SIG(fleury_startup)
CUSTOM_DOC("Fleury startup event") {
  ProfileScope(app, "default startup");
  
  User_Input input = get_current_input(app);
  if (!match_core_code(&input, CoreCode_Startup))
  {
    return;
  }
  
  //~ NOTE(rjf): Default 4coder initialization.
  String_Const_u8_Array file_names = input.event.core.file_names;
  load_themes_default_folder(app);
  default_4coder_initialize(app, file_names);
  
  //~ NOTE(rjf): Open special buffers.
  {
    // NOTE(rjf): Open compilation buffer.
    {
      Buffer_ID buffer = create_buffer(app, string_u8_litexpr("*compilation*"),
                                       BufferCreate_NeverAttachToFile |
                                       BufferCreate_AlwaysNew);
      buffer_set_setting(app, buffer, BufferSetting_Unimportant, true);
      buffer_set_setting(app, buffer, BufferSetting_ReadOnly, true);
    }
    
    // NOTE(rjf): Open lego buffer.
    {
      Buffer_ID buffer = create_buffer(app, string_u8_litexpr("*lego*"),
                                       BufferCreate_NeverAttachToFile |
                                       BufferCreate_AlwaysNew);
      buffer_set_setting(app, buffer, BufferSetting_Unimportant, true);
      buffer_set_setting(app, buffer, BufferSetting_ReadOnly, true);
    }
    
    // NOTE(rjf): Open calc buffer.
    {
      Buffer_ID buffer = create_buffer(app, string_u8_litexpr("*calc*"),
                                       BufferCreate_NeverAttachToFile |
                                       BufferCreate_AlwaysNew);
      buffer_set_setting(app, buffer, BufferSetting_Unimportant, true);
    }
    
    // NOTE(rjf): Open peek buffer.
    {
      Buffer_ID buffer = create_buffer(app, string_u8_litexpr("*peek*"),
                                       BufferCreate_NeverAttachToFile |
                                       BufferCreate_AlwaysNew);
      buffer_set_setting(app, buffer, BufferSetting_Unimportant, true);
    }
    
    // NOTE(rjf): Open LOC buffer.
    {
      Buffer_ID buffer = create_buffer(app, string_u8_litexpr("*loc*"),
                                       BufferCreate_NeverAttachToFile |
                                       BufferCreate_AlwaysNew);
      buffer_set_setting(app, buffer, BufferSetting_Unimportant, true);
    }
  }
  
  //~ NOTE(rjf): Initialize panels
  {
    Buffer_Identifier comp = buffer_identifier(string_u8_litexpr("*compilation*"));
    Buffer_Identifier left = buffer_identifier(string_u8_litexpr("*calc*"));
    Buffer_Identifier right = buffer_identifier(string_u8_litexpr("*messages*"));
    //Buffer_ID comp_id = buffer_identifier_to_id(app, comp);
    Buffer_ID left_id = buffer_identifier_to_id(app, left);
    Buffer_ID right_id = buffer_identifier_to_id(app, right);
    
    // NOTE(rjf): Left Panel
    View_ID view = get_active_view(app, Access_Always);
    new_view_settings(app, view);
    view_set_buffer(app, view, left_id, 0);
    
    // NOTE(rjf): Bottom panel
    /*View_ID compilation_view = 0;
    {
      compilation_view = open_view(app, view, ViewSplit_Bottom);
      new_view_settings(app, compilation_view);
      Buffer_ID buffer = view_get_buffer(app, compilation_view, Access_Always);
      Face_ID face_id = get_face_id(app, buffer);
      Face_Metrics metrics = get_face_metrics(app, face_id);
      view_set_split_pixel_size(app, compilation_view, (i32)(metrics.line_height * 4.f));
      view_set_passive(app, compilation_view, true);
      global_compilation_view = compilation_view;
      view_set_buffer(app, compilation_view, comp_id, 0);
    }
    */
    view_set_active(app, view);
    
    // NOTE(rjf): Right Panel
    open_panel_vsplit(app);
    
    View_ID right_view = get_active_view(app, Access_Always);
    view_set_buffer(app, right_view, right_id, 0);
    
    // NOTE(rjf): Restore Active to Left
    view_set_active(app, view);
  }
  
  //~ NOTE(rjf): Auto-Load Project.
  {
    b32 auto_load = def_get_config_b32(vars_save_string_lit("automatically_load_project"));
    if (auto_load)
    {
      load_project(app);
    }
  }
  
  //~ NOTE(rjf): Set misc options.
  {
    global_battery_saver = def_get_config_b32(vars_save_string_lit("f4_battery_saver"));
  }
  
  //~ NOTE(rjf): Initialize audio.
  {
    def_audio_init();
  }
  
  //~ NOTE(rjf): Initialize bindings.
  /*{
    String_Const_u8 bindings_file = string_u8_litexpr("bindings.4coder");
    F4_SetAbsolutelyNecessaryBindings(&framework_mapping);
    if (!dynamic_binding_load_from_file(app, &framework_mapping, bindings_file))
    {
      F4_SetDefaultBindings(&framework_mapping);
    }
    F4_SetAbsolutelyNecessaryBindings(&framework_mapping);
  }*/
  
  //~ NOTE(rjf): Initialize stylish fonts.
  {
    Scratch_Block scratch(app);
    String_Const_u8 bin_path = system_get_path(scratch, SystemPath_Binary);
    
    // NOTE(rjf): Fallback font.
    Face_ID face_that_should_totally_be_there = get_face_id(app, 0);
    
    // NOTE(rjf): Title font.
    {
      Face_Description desc = {0};
      {
        desc.font.file_name = push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
        desc.parameters.pt_size = 18;
        desc.parameters.bold = 0;
        desc.parameters.italic = 0;
        desc.parameters.hinting = 0;
      }
      
      if (IsFileReadable(desc.font.file_name)) {
        global_styled_title_face = try_create_new_face(app, &desc);
      }
      else {
        global_styled_title_face = face_that_should_totally_be_there;
      }
    }
    
    // NOTE(rjf): Label font.
    {
      Face_Description desc = {0};
      {
        desc.font.file_name = push_u8_stringf(scratch, "%.*sfonts/RobotoCondensed-Regular.ttf", string_expand(bin_path));
        desc.parameters.pt_size = 10;
        desc.parameters.bold = 1;
        desc.parameters.italic = 1;
        desc.parameters.hinting = 0;
      }
      
      if (IsFileReadable(desc.font.file_name)) {
        global_styled_label_face = try_create_new_face(app, &desc);
      }
      else {
        global_styled_label_face = face_that_should_totally_be_there;
      }
    }
    
    // NOTE(rjf): Small code font.
    {
      Face_Description normal_code_desc = get_face_description(app, get_face_id(app, 0));
      
      Face_Description desc = {0};
      {
        desc.font.file_name = push_u8_stringf(scratch, "%.*sfonts/Inconsolata-Regular.ttf", string_expand(bin_path));
        desc.parameters.pt_size = normal_code_desc.parameters.pt_size - 1;
        desc.parameters.bold = 1;
        desc.parameters.italic = 1;
        desc.parameters.hinting = 0;
      }
      
      if (IsFileReadable(desc.font.file_name)) {
        global_small_code_face = try_create_new_face(app, &desc);
      }
      else {
        global_small_code_face = face_that_should_totally_be_there;
      }
    }
  }
  
  //~ NOTE(rjf): Prep virtual whitespace.
  {
    def_enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
    clear_all_layouts(app);
  }
}
