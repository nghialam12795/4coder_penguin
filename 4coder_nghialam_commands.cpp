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

#if !defined(FCODER_NGHIALAM_COMMANDS)
#define FCODER_NGHIALAM_COMMAND

//~ NOTE(Nghia Lam): Main APIs
CUSTOM_COMMAND_SIG(nghialam_startup);             // Custom layer startup
CUSTOM_COMMAND_SIG(toggle_battery_saver);         // Change between batter saver mode <-> normal mode
CUSTOM_COMMAND_SIG(toggle_filebar_position);      // Change filebar on top <-> bottom position

//~ NOTE(Nghia Lam): My Implementation
CUSTOM_COMMAND_SIG(nghialam_startup)
CUSTOM_DOC("NghiaLam's custom layer startup event") {
  ProfileScope(app, "default startup");
  
  // Init startup code here
}

CUSTOM_COMMAND_SIG(toggle_battery_saver)
CUSTOM_DOC("Toggle battery saver mode") {
  global_battery_saver = !global_battery_saver;
}

CUSTOM_COMMAND_SIG(toggle_filebar_position)
CUSTOM_DOC("Toggle the position of filebar") {
  global_filebar_top = !global_filebar_top;
}

#endif // FCODER_NGHIALAM_COMMANDS