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

#include "4coder_penguin/4coder_penguin_vi_bindings.cpp"

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

//~ TODO(nghialam)
//
// [x] Vi/Vim modal key-binding
// [ ] Auto Snippets
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
    vim_init(app);

    vim_set_default_hooks(app);
    Vim_Key vim_leader_key = vim_key(KeyCode_Space); // Or whatever you prefer
    vim_setup_default_mapping(app, &framework_mapping, vim_leader_key);

    // NOTE: Open calc buffer.
    {
        Buffer_ID calc_buffer = create_buffer(app, string_u8_litexpr("*calc*"),
                                              BufferCreate_NeverAttachToFile |
                                              BufferCreate_AlwaysNew);
        buffer_set_setting(app, calc_buffer, BufferSetting_Unimportant, true);
    }
}

#endif // !FCODER_PENGUIN_  