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

#if !defined(FCODER_NGHIALAM_VIM)
#define FCODER_NGHIALAM_VIM


//~ NOTE(Nghia Lam): Type definitions and structures
enum VIMMODE {
  VIMMODE_NORMAL,
  VIMMODE_INSERT,
  VIMMODE_YANK,
  VIMMODE_GOTO,
  VIMMODE_VIEW,
  VIMMODE_DELETE,
  VIMMODE_VISUAL,
  VIMMODE_VISUALINSERT,
  VIMMODE_VISUALLINE,
  VIMMODE_VISUALBLOCK,
  VIMMODE_LEADER,
  VIMMODE_LEADER_BUFFER,
  VIMMODE_LEADER_WINDOW,
};

global VIMMODE global_vim_mode = VIMMODE_NORMAL;

//~ NOTE(Nghia Lam): Main APIs
function b32 NL_IsVimInsertMode(VIMMODE mode);
function b32 NL_IsVimVisualMode(VIMMODE mode);
function void NL_VimEnterMode(Application_Links *app, VIMMODE mode, b32 append = false);
function void NL_VimChangeMapID(Application_Links *app, Buffer_ID buffer, Command_Map_ID mapid);

//~ NOTE(Nghia Lam): My Implementation
function b32 NL_IsVimInsertMode(VIMMODE mode) {
  return ((mode == VIMMODE_INSERT) || (mode == VIMMODE_VISUALINSERT));
}

function b32 NL_IsVimVisualMode(VIMMODE mode) {
  return ((mode == VIMMODE_VISUAL)     || 
          (mode == VIMMODE_VISUALLINE) || 
          (mode == VIMMODE_VISUALBLOCK));
}

function void NL_VimEnterMode(Application_Links *app, VIMMODE mode, b32 append) {
  u32 access_flags = Access_ReadVisible;
  if (NL_IsVimInsertMode(mode)) {
    access_flags |= Access_Write;
  }
  
  // Is this really necessary?
  View_ID view = get_active_view(app, access_flags);
  Buffer_ID buffer = view_get_buffer(app, view, access_flags);
  if (!buffer_exists(app, buffer)) {
    return;
  }
  
  String_ID code_map_id = vars_save_string_lit("keys_code");
  
  String_ID vim_mapid_normal = vars_save_string_lit("keys_vim_normal");
  String_ID vim_mapid_visual = vars_save_string_lit("keys_vim_visual");
  String_ID vim_mapid_yank = vars_save_string_lit("keys_vim_yank");
  String_ID vim_mapid_goto = vars_save_string_lit("keys_goto");
  String_ID vim_mapid_view = vars_save_string_lit("keys_vim_view");
  String_ID vim_mapid_delete = vars_save_string_lit("keys_vim_delete");
  String_ID vim_mapid_leader = vars_save_string_lit("keys_vim_leader");
  String_ID vim_mapid_leader_buffer = vars_save_string_lit("keys_vim_leader_buffer");
  String_ID vim_mapid_leader_window = vars_save_string_lit("keys_vim_leader_window");
  
  global_vim_mode = mode;
  
  switch(mode) {
    case VIMMODE_NORMAL: {
      // TODO(Nghia Lam): Handle things for normal mode here
      NL_VimChangeMapID(app, buffer, vim_mapid_normal);
    } break;
    
    case VIMMODE_INSERT: {
      // TODO(Nghia Lam): Handle things for insert mode here
      NL_VimChangeMapID(app, buffer, code_map_id);
    } break;
    
    case VIMMODE_VISUALINSERT: {
      // TODO(Nghia Lam): Handle things for visual insert here
      NL_VimChangeMapID(app, buffer, code_map_id);
    } break;
    
    case VIMMODE_VISUAL: 
    case VIMMODE_VISUALLINE:
    case VIMMODE_VISUALBLOCK: {
      // TODO(Nghia Lam): Handle things for visual mode here
      NL_VimChangeMapID(app, buffer, vim_mapid_visual);
    } break;
    
    case VIMMODE_DELETE: {
      NL_VimChangeMapID(app, buffer, vim_mapid_delete);
    } break;
    
    case VIMMODE_YANK: {
      NL_VimChangeMapID(app, buffer, vim_mapid_yank);
    } break;
    
    case VIMMODE_GOTO: {
      NL_VimChangeMapID(app, buffer, vim_mapid_goto);
    } break;
    
    case VIMMODE_VIEW: {
      NL_VimChangeMapID(app, buffer, vim_mapid_view);
    } break;
    
    // NOTE(Nghia Lam): Custom vim keybindings
    case VIMMODE_LEADER: {
      NL_VimChangeMapID(app, buffer, vim_mapid_leader);
    } break;
    case VIMMODE_LEADER_BUFFER: {
      NL_VimChangeMapID(app, buffer, vim_mapid_leader_buffer);
    } break;
    case VIMMODE_LEADER_WINDOW: {
      NL_VimChangeMapID(app, buffer, vim_mapid_leader_window);
    } break;
  }
}

function void NL_VimChangeMapID(Application_Links *app, Buffer_ID buffer, Command_Map_ID mapid) {
  Managed_Scope scope        = buffer_get_managed_scope(app, buffer);
  Command_Map_ID* map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
  // NOTE(Nghia Lam): Assign map id here
  *map_id_ptr = mapid;
}

#endif // FCODER_NGHIALAM_VIM