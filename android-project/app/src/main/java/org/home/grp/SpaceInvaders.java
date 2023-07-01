package org.home.grp;

import org.libsdl.app.SDLActivity;
public class SpaceInvaders extends SDLActivity {

    protected String getMainFunction(){

        return "grp_Game";
    }

    protected String[] getLibraries() {
        return new String[] {
                "c++_shared",
                "SDL3",
                "engine", "game"
        };
    }

}
