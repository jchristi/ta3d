/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2005  Roland BROCHARD

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/

/*---------------------------------------------------------------------------------\
  |                                        gui.cpp                                   |
  |         Contient les fonctions nécessaires à la gestion de l'interface de ta3D   |
  |  comme les boutons, les fenêtres,...                                             |
  |                                                                                  |
  \---------------------------------------------------------------------------------*/

#include "stdafx.h"
#include "TA3D_NameSpace.h"
#include "misc/matrix.h"
#include "ta3dbase.h"
#include "misc/paths.h"
#include "languages/i18n.h"
#include "gfx/gui/skin.h"
#include "gfx/gui/skin.object.h"
#include "gfx/gui/area.h"
#include "misc/math.h"




namespace TA3D
{


bool use_normal_alpha_function = false;
float gui_font_h = 8.0f;


/*---------------------------------------------------------------------------\
  |        Dessine un bouton en (x,y) avec le texte Title,enfoncé si Etat=1    |
  \---------------------------------------------------------------------------*/

void button (float x,float y,float x2,float y2,const String &Title,bool Etat,float size, SKIN *skin )
{
    if( skin && skin->button_img[ Etat ].tex )					// If we have gfx to skin the button then do it
    {
        gfx->set_alpha_blending();
        gfx->set_color( 0xFFFFFFFF );

        skin->button_img[ Etat ].draw( x, y, x2, y2 );
        gfx->unset_alpha_blending();

        if (!Title.empty())
        {
            if (Etat)
                gfx->print(gui_font,(int)((x+x2)*0.5f-(gui_font.length(Title)*0.5f*size)+1),(int)((y+y2-(int)(gui_font.height()*size))*0.5f+1),0.0f,use_normal_alpha_function ? Blanc : Noir,Title,size);
            else
                gfx->print(gui_font,(int)((x+x2)*0.5f-(gui_font.length(Title)*0.5f*size)),(int)(y+y2-(int)(gui_font.height()*size))*0.5f,0.0f,use_normal_alpha_function ? Blanc : Noir,Title,size);
        }
    }
    else
    {
        gfx->rectfill(x,y,x2,y2,GrisM);
        gfx->rect(x,y,x2,y2,Noir);
        gfx->rect(x+1,y+1,x2-1,y2-1,GrisF);
        if(!Etat) {
            gfx->line(x,y,x2,y,Blanc);
            gfx->line(x,y,x,y2,Blanc);
            gfx->line(x+1,y+1,x2-1,y+1,GrisC);
            gfx->line(x+1,y+1,x+1,y2-1,GrisC);
            if( !Title.empty() )
                gfx->print(gui_font,(int)((x+x2)*0.5f-(gui_font.length(Title)*0.5f*size)),(int)(y+y2-(int)(gui_font.height()*size))*0.5f,0.0f,use_normal_alpha_function ? Blanc : Noir,Title,size);
        }
        else {
            gfx->line(x2,y2,x2,y,Blanc);
            gfx->line(x2,y2,x,y2,Blanc);
            gfx->line(x2-1,y2-1,x2-1,y+1,GrisC);
            gfx->line(x2-1,y2-1,x+1,y2-1,GrisC);
            if( !Title.empty() )
                gfx->print(gui_font,(int)((x+x2)*0.5f-(gui_font.length(Title)*0.5f*size)+1),(int)((y+y2-(int)(gui_font.height()*size))*0.5f+1),0.0f,use_normal_alpha_function ? Blanc : Noir,Title,size);
        }
    }
}

/*---------------------------------------------------------------------------\
  |        Draw a list box displaying the content of Entry                     |
  \---------------------------------------------------------------------------*/

void ListBox(float x1,float y1, float x2, float y2,const String::Vector &Entry,int Index, int Scroll, SKIN *skin, float size, uint32 flags )
{
    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    if (skin && skin->text_background.tex)
    {
        gfx->set_color( 0xFFFFFFFF );

        if( !(flags & FLAG_NO_BORDER) )
        {
            gfx->set_alpha_blending();
            skin->text_background.draw( x1, y1, x2, y2 );
            gfx->unset_alpha_blending();
        }

        int i;
        for( i = 0 ; i < Entry.size() ; i++ )
        {
            int e = i+Scroll;
            if (e >= Entry.size() || gui_font.height() * (i+1) > y2-y1-skin->text_background.y1+skin->text_background.y2) break;		// If we are out break the loop
            if (e == Index)
            {
                if (skin->selection_gfx.tex)
                    skin->selection_gfx.draw( x1 + skin->text_background.x1, y1 + skin->text_background.y1 + gui_font.height()*i, x2 + skin->text_background.x2, y1 + skin->text_background.y1+gui_font.height()*(i+1) );
                else
                    gfx->rectfill( x1 + skin->text_background.x1, y1 + skin->text_background.y1 + gui_font.height()*i, x2 + skin->text_background.x2, y1 + skin->text_background.y1+gui_font.height()*(i+1), Bleu );
            }
            String str = Entry[ e ];
            if (str.substr(0,3) == "<H>")       // Highlight this line
            {
                str = str.substr(3, str.size() - 3);
                glEnable( GL_BLEND );
                glDisable( GL_TEXTURE_2D );
                glBlendFunc( GL_ONE, GL_ONE );
                gfx->rectfill( x1 + skin->text_background.x1, y1 + skin->text_background.y1 + gui_font.height()*i, x2 + skin->text_background.x2, y1 + skin->text_background.y1+gui_font.height()*(i+1), makeacol( 0x7F, 0x7F, 0xFF, 0xFF ) );
                glDisable( GL_BLEND );
            }
            while( gui_font.length( str ) >= x2 - x1 - skin->text_background.x1 + skin->text_background.x2 - skin->scroll[0].sw && str.size() > 0 )
                str.resize( str.size() - 1 );
            gfx->print(gui_font,x1+skin->text_background.x1,y1+skin->text_background.y1+gui_font.height()*i,0.0f,use_normal_alpha_function ? Blanc : Noir,str);
        }

        int TotalScroll = Entry.size() - (int)( (y2 - y1 - skin->text_background.y1 + skin->text_background.y2) / gui_font.height() );
        if (TotalScroll < 0)	TotalScroll = 0;

        ScrollBar(	x2 + skin->text_background.x2 - skin->scroll[ 0 ].sw, y1 + skin->text_background.y1,
                    x2 + skin->text_background.x2, y2 + skin->text_background.y2,
                    TotalScroll ? ((float)Scroll) / TotalScroll : 0.0f, true, skin, size );
    }
    else
    {
        if (!(flags & FLAG_NO_BORDER))
        {
            gfx->rectfill(x1,y1,x2,y2,Blanc);
            gfx->rect(x1,y1,x2,y2,GrisF);
            gfx->rect(x1+1,y1+1,x2-1,y2-1,Noir);
            gfx->line(x1,y2,x2,y2,GrisC);
            gfx->line(x2,y1,x2,y2,GrisC);
            gfx->line(x1+1,y2-1,x2-1,y2-1,Blanc);
            gfx->line(x2-1,y1+1,x2-1,y2-1,Blanc);
        }

        int i;
        for(i=0;i<Entry.size();i++)
        {
            int e = i+Scroll;
            if (e >= Entry.size() || gui_font.height() * (i+1) > y2-y1-8) break;		// If we are out break the loop
            if (e == Index)
                gfx->rectfill( x1+4, y1+4+gui_font.height()*i, x2-4, y1+4+gui_font.height()*(i+1), Bleu );
            gfx->print(gui_font,x1+4,y1+4+gui_font.height()*i,0.0f,use_normal_alpha_function ? Blanc : Noir,Entry[e]);
        }
    }
    gui_font.change_size( old_size );
}

/*---------------------------------------------------------------------------\
  |        Draw a the given text within the given space                        |
  \---------------------------------------------------------------------------*/

int draw_text_adjust( float x1, float y1, float x2, float y2, String msg, float size, int pos, bool mission_mode )
{
    String current = "";
    String current_word = "";
    std::vector< String > Entry;
    int last = 0;
    for( int i = 0 ; i < msg.length() ; i++ )
        if (msg[i] == '\r')	continue;
        else if (msg[i] == '\n' || gui_font.length( current + ' ' + current_word + msg[i] ) * size >= x2 - x1)
        {
            bool line_too_long = true;
            if (gui_font.length( current + ' ' + current_word + msg[i] ) * size < x2 - x1)
            {
                current += ' ' + current_word;
                current_word.clear();
                line_too_long = false;
            }
            else if (msg[i] != '\n')
                current_word += msg[i];
            Entry.push_back( current );
            last = i + 1;
            current.clear();
            if( msg[ i ] == '\n' && line_too_long ) {
                Entry.push_back( current_word );
                current_word.clear();
            }
        }
        else
        {
            if( msg[i] == ' ' )
            {
                if( !current.empty() )
                    current += ' ';
                current += current_word;
                current_word.clear();
            }
            else
                current_word += msg[i];
        }

    if (!current.empty())
        current << ' ' << current_word;
    else
        current << current_word;

    if( last + 1 < msg.length() && !current.empty() )
        Entry.push_back( current );

    gfx->set_color( 0xFFFFFFFF );

    if (mission_mode)
    {
        int	old_format = get_uformat();
        set_uformat( U_ASCII );

        uint32	current_color = 0xFFFFFFFF;
        char tmp[2];
        tmp[1] = 0;
        for( int e = pos ; e < Entry.size() ; e++ )
            if (y1 + gui_font.height() * size * (e + 1 - pos) <= y2)
            {
                float x_offset = 0.0f;
                for( int i = 0 ; i < Entry[e].size() ; i++ )
                    if (Entry[e][i] == '&')
                    {
                        current_color = 0xFFFFFFFF;									// Default: white
                        if (i + 1 < Entry[e].size() && Entry[e][i+1] == 'R')
                        {
                            current_color = 0xFF0000FF;								// Red
                            i++;
                        }
                        else if (i + 1 < Entry[e].size() && Entry[e][i+1] == 'Y')
                        {
                            current_color = 0xFF00FFFF;								// Yellow
                            i++;
                        }
                    }
                    else
                    {
                        tmp[0] = Entry[e][i];
                        gfx->print( gui_font, x1 + x_offset, y1 + gui_font.height() * size * (e - pos), 0.0f, current_color, tmp, size );
                        x_offset += gui_font.length( tmp ) * size;
                    }
            }
        set_uformat( old_format );
    }
    else
    {
        for( int e = pos ; e < Entry.size() ; e++ )
            if( y1 + gui_font.height() * size * (e + 1 - pos) <= y2 )
                gfx->print( gui_font, x1, y1 + gui_font.height() * size * (e - pos), 0.0f, use_normal_alpha_function ? Blanc : Noir, Entry[e], size );
    }

    return Entry.size();
}

/*---------------------------------------------------------------------------\
  |        Draw a popup menu displaying the text msg using the skin object     |
  \---------------------------------------------------------------------------*/

void PopupMenu( float x1, float y1, const String &msg, SKIN *skin, float size )
{
    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    float x2 = x1;
    std::vector< String > Entry;
    int last = 0;
    for( int i = 0 ; i < msg.length() ; i++ )
        if (msg[i] == '\n')
        {
            Entry.push_back( msg.substr( last, i - last ) );
            x2 = Math::Max(x2, x1 + gui_font.length(Entry.back()));
            last = i+1;
        }
    if (last + 1 < msg.length())
    {
        Entry.push_back( msg.substr( last, msg.length() - last));
        x2 = Math::Max(x2, x1 + gui_font.length(Entry.back()));
    }

    if (skin && skin->menu_background.tex)
    {
        x2 += skin->menu_background.x1 - skin->menu_background.x2;
        float y2 = y1+skin->menu_background.y1-skin->menu_background.y2+gui_font.height()*Entry.size();
        if (x2 >= SCREEN_W)
        {
            x1 += SCREEN_W - x2 - 1;
            x2 = SCREEN_W - 1;
        }
        if (y2 >= SCREEN_H)
        {
            y1 += SCREEN_H - y2 - 1;
            y2 = SCREEN_H - 1;
        }
        gfx->set_alpha_blending();
        gfx->set_color( 0xFFFFFFFF );

        skin->menu_background.draw( x1, y1, x2, y2 );
        gfx->unset_alpha_blending();

        for( int e = 0 ; e < Entry.size() ; e++ )
            gfx->print(gui_font,x1+skin->menu_background.x1,y1+skin->menu_background.y1+gui_font.height()*e,0.0f,use_normal_alpha_function ? Blanc : Noir,Entry[e]);
    }
    else
    {
        x2 += 8;
        float y2 = y1+8+gui_font.height()*Entry.size();
        if (x2 >= SCREEN_W || y2 >= SCREEN_H)
        {
            float x = x1, y = y1;
            x1 = x1 - (x2 - x1);
            y1 = y1 - (y2 - y1);
            x2 = x;
            y2 = y;
        }
        gfx->rectfill(x1,y1,x2,y2,GrisM);
        gfx->rect(x1,y1,x2,y2,Noir);
        gfx->rect(x1+1,y1+1,x2-1,y2-1,GrisF);
        gfx->line(x1,y1,x1,y2,Blanc);
        gfx->line(x1,y1,x2,y1,Blanc);
        gfx->line(x1+1,y1+1,x1+1,y2-1,GrisC);
        gfx->line(x1+1,y1+1,x2-1,y1+1,GrisC);

        for( int e = 0 ; e < Entry.size() ; e++ )
            gfx->print(gui_font,x1+4,y1+4+gui_font.height()*e,0.0f,use_normal_alpha_function ? Blanc : Noir,Entry[e]);
    }
    Entry.clear();

    gui_font.change_size( old_size );
}

/*---------------------------------------------------------------------------\
  |        Dessine un menu flotant avec les paramètres de Entry[]              |
  \---------------------------------------------------------------------------*/

void FloatMenu( float x, float y, const String::Vector &Entry, int Index, int StartEntry, SKIN *skin, float size )
{
    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    if (skin && skin->menu_background.tex)
    {
        gfx->set_color( 0xFFFFFFFF );

        int i;
        float width = 168.0f * size;
        for (i = 0; i < Entry.size() - StartEntry; ++i)
            width = Math::Max(width, gui_font.length(Entry[i]));

        width += skin->menu_background.x1-skin->menu_background.x2;

        gfx->set_alpha_blending();
        skin->menu_background.draw( x, y, x + width, y+skin->menu_background.y1-skin->menu_background.y2+gui_font.height()*(Entry.size() - StartEntry) );

        for (i = 0; i < Entry.size() - StartEntry; ++i)
        {
            int e = i + StartEntry;
            if( e == Index )
            {
                if( skin->selection_gfx.tex )
                    skin->selection_gfx.draw( x+skin->menu_background.x1,y+skin->menu_background.y1+gui_font.height()*i,x + width + skin->menu_background.x2,y+skin->menu_background.y1+gui_font.height()*(i+1) );
                else
                    gfx->rectfill( x+skin->menu_background.x1,y+skin->menu_background.y1+gui_font.height()*i,x + width + skin->menu_background.x2,y+skin->menu_background.y1+gui_font.height()*(i+1), Bleu );
            }
            gfx->print(gui_font,x+skin->menu_background.x1,y+skin->menu_background.y1+gui_font.height()*i,0.0f,use_normal_alpha_function ? Blanc : Noir,Entry[e]);
        }
        gfx->unset_alpha_blending();
    }
    else
    {
        gfx->rectfill(x,y,x+168,y+8+gui_font.height()*(Entry.size() - StartEntry),GrisM);
        gfx->rect(x,y,x+168,y+8+gui_font.height()*(Entry.size() - StartEntry),Noir);
        gfx->rect(x+1,y+1,x+167,y+7+gui_font.height()*(Entry.size() - StartEntry),GrisF);
        gfx->line(x,y,x,y+8+gui_font.height()*(Entry.size() - StartEntry),Blanc);
        gfx->line(x,y,x+168,y,Blanc);
        gfx->line(x+1,y+1,x+1,y+7+gui_font.height()*(Entry.size() - StartEntry),GrisC);
        gfx->line(x+1,y+1,x+167,y+1,GrisC);

        int i;
        for( i=0 ; i<Entry.size() - StartEntry ; i++ )
        {
            int e = i + StartEntry;
            if( e == Index )
                gfx->rectfill( x+4,y+4+gui_font.height()*i,x+164,y+4+gui_font.height()*(i+1), Bleu );
            gfx->print(gui_font,x+4,y+4+gui_font.height()*i,0.0f,use_normal_alpha_function ? Blanc : Noir,Entry[e]);
        }
    }

    gui_font.change_size( old_size );
}

/*---------------------------------------------------------------------------\
  |        Dessine un boutton d'option avec le texte Title                     |
  \---------------------------------------------------------------------------*/

void OptionButton(float x,float y,const String &Title,bool Etat, SKIN *skin, float size )
{
    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    if (skin && skin->option[0].tex && skin->option[1].tex)
    {
        gfx->set_color( 0xFFFFFFFF );
        gfx->set_alpha_blending();

        skin->option[ Etat ? 1 : 0 ].draw( x, y, x + skin->option[ Etat ? 1 : 0 ].sw, y + skin->option[ Etat ? 1 : 0 ].sh );
        gfx->unset_alpha_blending();

        gfx->print(gui_font, x + skin->option[ Etat ? 1 : 0 ].sw + 4.0f * size, y + ( skin->option[ Etat ? 1 : 0 ].sh - gui_font.height() ) * 0.5f,0.0f,use_normal_alpha_function ? Blanc : Noir,Title);
    }
    else
    {
        int x1,y1;

        gfx->circlefill(x+6,y+6,6,Blanc);
        gfx->circle(x+6,y+6,6,GrisM);
        gfx->circle(x+6,y+6,5,GrisF);

        // Effet d'ombrage
        for(y1=(int)y ; y1<=y+12 ; y1++)
            for(x1=(int)x ; x1<=x+12 ; x1++)
                if((x1-x-6)*(x1-x-6)+(y1-y-6)*(y1-y-6)<49)
                    if(gfx->getpixel(x1,y1)==GrisF&&x1-x+y1-y-6>6)
                        gfx->putpixel(x1,y1,GrisC);

        if(Etat)
        {
            gfx->putpixel(x+6,y+6,RougeF);
            gfx->circle(x+6,y+6,1,RougeF);
            gfx->circle(x+6,y+6,2,RougeF);
            gfx->circle(x+6,y+6,3,RougeF);
            gfx->circlefill(x+6,y+6,2,RougeF);
            gfx->putpixel(x+6,y+6,Rouge);
            gfx->circle(x+6,y+6,1,Rouge);
            gfx->circle(x+6,y+6,2,Rouge);
            gfx->circlefill(x+6,y+6,2,Rouge);
        }

        gfx->print(gui_font,x+16,y+3,0.0f,use_normal_alpha_function ? Blanc : Noir,Title);
    }

    gui_font.change_size( old_size );
}

/*---------------------------------------------------------------------------\
  |        Dessine une case à cocher avec le texte Title                       |
  \---------------------------------------------------------------------------*/

void OptionCase(float x,float y,const String &Title,bool Etat, SKIN *skin, float size )
{
    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    if (skin && skin->checkbox[0].tex && skin->checkbox[1].tex)
    {
        gfx->set_color( 0xFFFFFFFF );
        gfx->set_alpha_blending();

        skin->checkbox[ Etat ? 1 : 0 ].draw( x, y, x + skin->checkbox[ Etat ? 1 : 0 ].sw, y + skin->checkbox[ Etat ? 1 : 0 ].sh );
        gfx->unset_alpha_blending();

        gfx->print(gui_font, x + skin->checkbox[ Etat ? 1 : 0 ].sw + 4.0f * size, y + ( skin->checkbox[ Etat ? 1 : 0 ].sh - gui_font.height() ) * 0.5f,0.0f,use_normal_alpha_function ? Blanc : Noir,Title);
    }
    else
    {
        gfx->rectfill(x,y,x+12,y+12,Blanc);
        gfx->rect(x,y,x+12,y+12,GrisF);
        gfx->rect(x+1,y+1,x+11,y+11,Noir);
        gfx->line(x,y+12,x+12,y+12,GrisC);
        gfx->line(x+12,y,x+12,y+12,GrisC);
        gfx->line(x+1,y+11,x+11,y+11,Blanc);
        gfx->line(x+11,y+1,x+11,y+11,Blanc);

        if (Etat)
        {
            gfx->line(x+3,y+6,x+5,y+9,GrisF);
            gfx->line(x+5,y+9,x+8,y+4,GrisF);
            gfx->line(x+3,y+5,x+5,y+8,GrisF);
            gfx->line(x+5,y+8,x+8,y+3,GrisF);
        }

        gfx->print(gui_font,x+16,y+3,0.0f,use_normal_alpha_function ? Blanc : Noir,Title);
    }

    gui_font.change_size( old_size );
}

/*---------------------------------------------------------------------------\
  |        Draw a TEXTEDITOR widget (a large text input widget)                |
  \---------------------------------------------------------------------------*/

void TextEditor( float x1, float y1, float x2, float y2, const String::Vector &Entry, int row, int col, bool Etat, SKIN *skin, float size )
{
    int old_u_format = get_uformat();
    set_uformat( U_ASCII );

    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    bool blink = Etat && (msec_timer % 1000) >= 500;

    if (skin && skin->text_background.tex)
    {
        gfx->set_color( 0xFFFFFFFF );
        gfx->set_alpha_blending();

        skin->text_background.draw( x1, y1, x2, y2 );
        gfx->unset_alpha_blending();

        float maxlength = x2 - x1 + skin->text_background.x2 - skin->text_background.x1 - gui_font.length( "_" );
        float maxheight = y2 - y1 + skin->text_background.y2 - skin->text_background.y1 - skin->text_y_offset;
        int H = Math::Max( row - (int)(0.5f * maxheight / gui_font.height()), 0 );
        int y = 0;
        while (gui_font.height() * (y+1) <= maxheight && y + H < Entry.size())
        {
            float xdec = -1.0f;
            String strtoprint;
            for (int x = 0; x < Entry[y+H].size() ; x++)
            {
                if (gui_font.length( strtoprint + Entry[y+H][x]) > maxlength)
                {
                    gfx->print( gui_font,x1+skin->text_background.x1,
                                y1+skin->text_background.y1+skin->text_y_offset+gui_font.height() * y,
                                0.0f,use_normal_alpha_function ? Blanc : Noir,strtoprint);
		            if (xdec >= 0.0f)
		                gfx->print( gui_font,x1+skin->text_background.x1+xdec,
		                            y1+skin->text_background.y1+skin->text_y_offset+gui_font.height() * y,
		                            0.0f,use_normal_alpha_function ? Blanc : Noir,"_");
		            xdec = -1.0f;
		            y++;
		            H--;
		            strtoprint.clear();
		            if (gui_font.height() * (y+1) >= maxheight)    break;
                }
                if (row == y+H && x == col && blink)
                    xdec = gui_font.length( strtoprint );
                strtoprint += Entry[y+H][x];
            }
            gfx->print( gui_font,x1+skin->text_background.x1,
                        y1+skin->text_background.y1+skin->text_y_offset+gui_font.height() * y,
                        0.0f,use_normal_alpha_function ? Blanc : Noir,strtoprint);
            if (y+H == row && col == Entry[row].size() && blink)
                xdec = gui_font.length( strtoprint );
		    if (xdec >= 0.0f)
		        gfx->print( gui_font,x1+skin->text_background.x1+xdec,
		                    y1+skin->text_background.y1+skin->text_y_offset+gui_font.height() * y,
		                    0.0f,use_normal_alpha_function ? Blanc : Noir,"_");
            y++;
        }
    }

    gui_font.change_size( old_size );

    set_uformat(old_u_format);
}

/*---------------------------------------------------------------------------\
  |        Dessine une barre d'entrée de texte utilisateur                     |
  \---------------------------------------------------------------------------*/

void TextBar(float x1,float y1,float x2,float y2,const String &Caption,bool Etat, SKIN *skin, float size )
{
    int old_u_format = get_uformat();
    set_uformat( U_ASCII );

    float old_size = gui_font.get_size();
    gui_font.change_size( size );
    bool blink = Etat && (msec_timer % 1000) >= 500;

    if (skin && skin->text_background.tex)
    {
        gfx->set_color( 0xFFFFFFFF );
        gfx->set_alpha_blending();

        skin->text_background.draw( x1, y1, x2, y2 );
        gfx->unset_alpha_blending();

        float maxlength = x2 - x1 + skin->text_background.x2 - skin->text_background.x1 - gui_font.length( "_" );
        int dec = 0;
        String strtoprint = Caption.substr( dec, Caption.length() - dec );
        while (gui_font.length( Caption.substr( dec, Caption.length() - dec ) ) >= maxlength && dec < Caption.length())
        {
            dec++;
            strtoprint = Caption.substr( dec, Caption.length() - dec );
        }

        gfx->print(gui_font,x1+skin->text_background.x1,y1+skin->text_background.y1+skin->text_y_offset,0.0f,use_normal_alpha_function ? Blanc : Noir,strtoprint);
		if (blink) gfx->print(gui_font,x1+skin->text_background.x1+gui_font.length( strtoprint ),y1+skin->text_background.y1+skin->text_y_offset,0.0f,use_normal_alpha_function ? Blanc : Noir,"_");
    }
    else
    {
        gfx->rectfill(x1,y1,x2,y2,Blanc);
        gfx->rect(x1,y1,x2,y2,GrisF);
        gfx->rect(x1+1,y1+1,x2-1,y2-1,Noir);
        gfx->line(x1,y2,x2,y2,GrisC);
        gfx->line(x2,y1,x2,y2,GrisC);
        gfx->line(x1+1,y2-1,x2-1,y2-1,Blanc);
        gfx->line(x2-1,y1+1,x2-1,y2-1,Blanc);

        float maxlength = x2 - x1 - 8 - gui_font.length( "_" );
        int dec = 0;
        String strtoprint = Caption.substr( dec, Caption.length() - dec );
        while( gui_font.length( Caption.substr( dec, Caption.length() - dec ) ) >= maxlength && dec < Caption.length() )
        {
            dec++;
            strtoprint = Caption.substr( dec, Caption.length() - dec );
        }

        gfx->print(gui_font,x1+4,y1+4,0.0f,use_normal_alpha_function ? Blanc : Noir,Caption);
        if (blink) gfx->print(gui_font,x1+4+gui_font.length( Caption ),y1+4,0.0f,use_normal_alpha_function ? Blanc : Noir,"_");
    }

    gui_font.change_size( old_size );

    set_uformat(old_u_format);
}

/*---------------------------------------------------------------------------\
  |                              Draw a scroll bar                             |
  \---------------------------------------------------------------------------*/
void ScrollBar( float x1, float y1, float x2, float y2, float Value, bool vertical, SKIN *skin, float size )
{
    if (skin)
    {
        gfx->set_color( 0xFFFFFFFF );
        gfx->set_alpha_blending();

        if( Value < 0.0f )	Value = 0.0f;
        else if( Value > 1.0f )	Value = 1.0f;
        skin->scroll[ vertical ? 0 : 1 ].draw( x1, y1, x2, y2 );

        if (vertical)
        {
            float y = y1 + skin->scroll[ 0 ].y1;
            float dx = x2 - x1 - skin->scroll[ 0 ].x1 + skin->scroll[ 0 ].x2;
            y += (y2 - y1 - skin->scroll[ 0 ].y1 + skin->scroll[ 0 ].y2 - dx) * Value;
            skin->scroll[ 2 ].draw( x1 + skin->scroll[ 0 ].x1, y, x2 + skin->scroll[ 0 ].x2, y + dx );
        }
        else
        {
            float x = x1 + skin->scroll[ 1 ].x1;
            float dy = y2 - y1 - skin->scroll[ 1 ].y1 + skin->scroll[ 1 ].y2;
            x += (x2 - x1 - skin->scroll[ 1 ].x1 + skin->scroll[ 1 ].x2 - dy) * Value;
            skin->scroll[ 2 ].draw( x, y1 + skin->scroll[ 1 ].y1, x + dy, y2 + skin->scroll[ 1 ].y2 );
        }
        gfx->unset_alpha_blending();
    }
}

/*---------------------------------------------------------------------------\
  |                     Dessine une barre de progression                       |
  \---------------------------------------------------------------------------*/

void ProgressBar(float x1,float y1,float x2,float y2,int Value, SKIN *skin, float size )
{
    float old_size = gui_font.get_size();
    gui_font.change_size( size );

    if (skin && skin->progress_bar[0].tex && skin->progress_bar[1].tex)			// If we have a skin loaded with gfx for the progress bar
    {
        gfx->set_color( 0xFFFFFFFF );
        gfx->set_alpha_blending();
        skin->progress_bar[0].draw( x1, y1, x2, y2 );
        skin->progress_bar[1].draw( x1 + skin->progress_bar[0].x1, y1 + skin->progress_bar[0].y1, x1 + skin->progress_bar[0].x1 + (skin->progress_bar[0].x2 + x2 - x1 - skin->progress_bar[0].x1) * Value * 0.01f, y2 + skin->progress_bar[0].y2 );			// Draw the bar
        gfx->unset_alpha_blending();

        String Buf = format("%d", Value) + "%%";

        gfx->print(gui_font,(x1+x2)*0.5f-gui_font.length( Buf ) * 0.5f,(y1+y2)*0.5f-gui_font.height()*0.5f,0.0f,use_normal_alpha_function ? Blanc : Noir,Buf);
    }
    else
    {
        gfx->rectfill(x1,y1,x2,y2,Blanc);
        gfx->rectfill(x1+2,y1,x1+1+(x2-x1-3)*Value/100,y2,Bleu);
        gfx->rect(x1,y1,x2,y2,GrisF);
        gfx->rect(x1+1,y1+1,x2-1,y2-1,Noir);
        gfx->line(x1,y2,x2,y2,GrisC);
        gfx->line(x2,y1,x2,y2,GrisC);
        gfx->line(x1+1,y2-1,x2-1,y2-1,Blanc);
        gfx->line(x2-1,y1+1,x2-1,y2-1,Blanc);

        String Buf = format("%d", Value) + "%%";

        gfx->print(gui_font,(x1+x2)/2-gui_font.length( Buf ) * 0.5f,(y1+y2)*0.5f-gui_font.height()*0.5f,0.0f,use_normal_alpha_function ? Blanc : Noir,Buf);
    }

    gui_font.change_size( old_size );
}

String curDir = "";

/*---------------------------------------------------------------------------\
  |               Affiche une fenêtre de selection de fichier                  |
  \---------------------------------------------------------------------------*/

const String Dialogf(const String &Title, String Filter)
{
    AREA *current_area = AREA::current();
    String result = "";

    if (current_area)
    {
        if (current_area->get_wnd( "open" ) == NULL)            // The window isn't loaded => load it now !
            current_area->load_window( "gui/open_dialog.tdf" );
        current_area->set_title("open",Title);

        if (curDir.empty())                         // If empty grab current directory
            curDir = TA3D::Paths::CurrentDirectory() + TA3D::Paths::Separator;

        String::List files, dirs;

        TA3D::Paths::GlobFiles( files, curDir + Filter );
        TA3D::Paths::GlobDirs( dirs, curDir + "*" );
        TA3D::Paths::ExtractFileName( files );
        TA3D::Paths::ExtractFileName( dirs );

        files.sort();
        dirs.sort();

        current_area->set_entry("open.file_list", files);
        current_area->set_entry("open.folder_list", dirs);
        current_area->msg("open.show");

        current_area->set_state("open.b_ok", false);        // We don't want to leave right now
        current_area->set_state("open.b_cancel", false);

        bool done = false;
        int amx, amy, amz, amb;
        int cur_folder_idx = -1;

        do
        {
            bool key_is_pressed = false;
            do
            {
                amx = mouse_x;
                amy = mouse_y;
                amz = mouse_z;
                amb = mouse_b;

                key_is_pressed = keypressed();
                current_area->check();
                rest( 8 );
            } while( amx == mouse_x && amy == mouse_y && amz == mouse_z && amb == mouse_b && !key[ KEY_ENTER ] && !key[ KEY_ESC ] && !done && !key_is_pressed && !current_area->scrolling );

            if (key[KEY_ESC] || current_area->get_state("open.b_cancel"))   done = true;

            if (key[KEY_ENTER] || current_area->get_state("open.b_ok"))
            {
                done = true;
                result = curDir + TA3D::Paths::Separator + current_area->get_caption("open.t_filename");
            }

            if (current_area->get_state("open.file_list"))
            {
                GUIOBJ *obj = current_area->get_object("open.file_list");
                if (obj && obj->Pos >= 0 && obj->Pos < obj->Text.size())
                {
                    if (current_area->get_caption("open.t_filename") == obj->Text[ obj->Pos ])      // Double-click
                    {
                        done = true;
                        result = curDir + TA3D::Paths::Separator + current_area->get_caption("open.t_filename");
                    }
                    else
                        current_area->set_caption("open.t_filename", obj->Text[ obj->Pos ]);
                }
            }

            if (current_area->get_state("open.folder_list"))
            {
                GUIOBJ *obj = current_area->get_object("open.folder_list");
                if (obj && obj->Pos >= 0 && obj->Pos < obj->Text.size())
                {
                    if (obj->Pos == cur_folder_idx)     // Change current dir (double-click)
                    {
                        if (obj->Text[ obj->Pos ] == "..")
                            curDir = TA3D::Paths::ExtractFilePath( curDir );
                        else
                            curDir << obj->Text[ obj->Pos ] << TA3D::Paths::Separator;
                        TA3D::Paths::GlobFiles( files, curDir + Filter );
                        TA3D::Paths::GlobDirs( dirs, curDir + "*" );
                        TA3D::Paths::ExtractFileName( files );
                        TA3D::Paths::ExtractFileName( dirs );

                        files.sort();
                        dirs.sort();

                        current_area->set_entry("open.file_list", files);
                        current_area->set_entry("open.folder_list", dirs);
                        cur_folder_idx = -1;
                    }
                    else
                        cur_folder_idx = obj->Pos;
                }
            }

            gfx->SetDefState();
            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gfx->set_2D_mode();		// Passe en mode dessin allegro

            current_area->draw();
            show_mouse(screen);
            algl_draw_mouse();

            gfx->unset_2D_mode();	// Quitte le mode de dessin d'allegro
            gfx->flip();

        }while(!done);

        current_area->msg("open.hide");
    }

    reset_keyboard();

    return result;
}

/*---------------------------------------------------------------------------\
  |               Affiche une fenêtre de demande de type oui/non               |
  \---------------------------------------------------------------------------*/

bool WndAsk(const String &Title,const String &Msg,int ASW_TYPE)
{
    AREA *current_area = AREA::current();

    if (current_area)
    {
        if (current_area->get_wnd( "yesno" ) == NULL)            // The window isn't loaded => load it now !
            current_area->load_window( "gui/yesno_dialog.tdf" );
        current_area->set_title("yesno",Title);

        current_area->msg("yesno.show");
        current_area->set_caption("yesno.msg", Msg);

        // Boutons OK/Oui et Annuler/Non
        if (ASW_TYPE==ASW_OKCANCEL)
        {
            current_area->set_caption("yesno.b_ok",I18N::Translate("OK"));
            current_area->set_caption("yesno.b_cancel",I18N::Translate("Cancel"));
        }
        else
        {
            current_area->set_caption("yesno.b_ok",I18N::Translate("Yes"));
            current_area->set_caption("yesno.b_cancel",I18N::Translate("No"));
        }

        bool done = false;
        int amx, amy, amz, amb;
        bool answer = false;

        do
        {
            bool key_is_pressed = false;
            do
            {
                amx = mouse_x;
                amy = mouse_y;
                amz = mouse_z;
                amb = mouse_b;

                key_is_pressed = keypressed();
                current_area->check();
                rest( 8 );
            } while( amx == mouse_x && amy == mouse_y && amz == mouse_z && amb == mouse_b && !key[ KEY_ENTER ] && !key[ KEY_ESC ] && !done && !key_is_pressed && !current_area->scrolling );

            if (key[KEY_ESC] || current_area->get_state("yesno.b_cancel"))
            {
                done = true;
                answer = false;
            }

            if (key[KEY_ENTER] || current_area->get_state("yesno.b_ok"))
            {
                done = true;
                answer = true;
            }

            gfx->SetDefState();
            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gfx->set_2D_mode();		// Passe en mode dessin allegro

            current_area->draw();
            show_mouse(screen);
            algl_draw_mouse();

            gfx->unset_2D_mode();	// Quitte le mode de dessin d'allegro
            gfx->flip();

        }while(!done);
        current_area->msg("yesno.hide");

        reset_keyboard();

        return answer;
    }

    return false;
}

/*---------------------------------------------------------------------------\
  |               Affiche une fenêtre Popup d'affichage d'infos                |
  \---------------------------------------------------------------------------*/

void Popup(const String &Title,const String &Msg)
{
    AREA *current_area = AREA::current();

    if (current_area)
    {
        if (current_area->get_wnd( "popup" ) == NULL)            // The window isn't loaded => load it now !
            current_area->load_window( "gui/popup_dialog.tdf" );
        current_area->set_title("popup",Title);

        current_area->msg("popup.show");
        current_area->set_caption("popup.msg", Msg);

        bool done = false;
        int amx, amy, amz, amb;

        do
        {
            bool key_is_pressed = false;
            do
            {
                amx = mouse_x;
                amy = mouse_y;
                amz = mouse_z;
                amb = mouse_b;

                key_is_pressed = keypressed();
                current_area->check();
                rest( 8 );
            } while( amx == mouse_x && amy == mouse_y && amz == mouse_z && amb == mouse_b && !key[ KEY_ENTER ] && !key[ KEY_ESC ] && !done && !key_is_pressed && !current_area->scrolling );

            if (key[KEY_ESC])   done = true;

            if (key[KEY_ENTER] || current_area->get_state("popup.b_ok"))
                done = true;

            gfx->SetDefState();
            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gfx->set_2D_mode();		// Passe en mode dessin allegro

            current_area->draw();
            show_mouse(screen);
            algl_draw_mouse();

            gfx->unset_2D_mode();	// Quitte le mode de dessin d'allegro
            gfx->flip();

        }while(!done);
        current_area->msg("popup.hide");

        reset_keyboard();
    }
}

/*---------------------------------------------------------------------------\
  |               Affiche une fenêtre de demande d'entrée utilisateur          |
  \---------------------------------------------------------------------------*/

const String GetVal(const String &Title)
{
    AREA *current_area = AREA::current();
    String result = "";

    if (current_area)
    {
        if (current_area->get_wnd( "ask" ) == NULL)            // The window isn't loaded => load it now !
            current_area->load_window( "gui/ask_dialog.tdf" );
        current_area->set_title("ask",Title);

        current_area->msg("ask.show");
        current_area->msg("ask.t_result.focus");

        bool done = false;
        int amx, amy, amz, amb;

        do
        {
            bool key_is_pressed = false;
            do
            {
                amx = mouse_x;
                amy = mouse_y;
                amz = mouse_z;
                amb = mouse_b;

                key_is_pressed = keypressed();
                current_area->check();
                rest( 8 );
            } while( amx == mouse_x && amy == mouse_y && amz == mouse_z && amb == mouse_b && !key[ KEY_ENTER ] && !key[ KEY_ESC ] && !done && !key_is_pressed && !current_area->scrolling );

            if (key[KEY_ESC] || current_area->get_state("ask.b_cancel"))   done = true;

            if (key[KEY_ENTER] || current_area->get_state("ask.b_ok"))
            {
                done = true;
                result = current_area->get_caption("ask.t_result");
            }

            gfx->SetDefState();
            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gfx->set_2D_mode();		// Passe en mode dessin allegro

            current_area->draw();
            show_mouse(screen);
            algl_draw_mouse();

            gfx->unset_2D_mode();	// Quitte le mode de dessin d'allegro
            gfx->flip();

        }while(!done);
        current_area->msg("ask.hide");
    }

    reset_keyboard();

    return result;
}

} // namespace TA3D
