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

// NOTE(Nghia Lam): The order of included files does matter when building
#include "4coder_default_include.cpp"

#include "4coder_nghialam/4coder_nghialam_base.cpp"
#include "4coder_nghialam/4coder_nghialam_helpers.cpp"
#include "4coder_nghialam/4coder_nghialam_vim.cpp"
#include "4coder_nghialam/4coder_nghialam_draw.cpp"
#include "4coder_nghialam/4coder_nghialam_hook.cpp"
#include "4coder_nghialam/4coder_nghialam_commands.cpp"

//~ TODO(Nghia Lam): Create my custom layer
//  - [ ] VI Model Editing System ??
//  - [ ] Plot comment system
//  - [ ] Stuff for rendering
//  - [ ] My own file bar rendering
//  - [ ] Project management
//  - [ ] Project todo list

//~ NOTE(Nghia Lam): Entry Point here
void custom_layer_init(Application_Links *app) {
  Thread_Context *tctx = get_thread_context(app);
  
  // NOTE(Allen): Setup for default framework
  default_framework_init(app);
  
  // NOTE(Nghia Lam): Custom hooks
  set_all_default_hooks(app);
  set_custom_hook(app, HookID_Tick,                    NL_Tick);
  set_custom_hook(app, HookID_RenderCaller,            NL_RenderCaller);
  set_custom_hook(app, HookID_WholeScreenRenderCaller, NL_WholeScreenRenderCaller);
  
  // NOTE(Nghia Lam): Binding setup
  mapping_init(tctx, &framework_mapping);
  setup_default_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
}