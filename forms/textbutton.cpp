
#include "textbutton.h"

RawSound* TextButton::buttonclick = nullptr;

TextButton::TextButton( Control* Owner, std::string Text, ApocalypseFont* Font ) : Control( Owner ), text( Text ), font( Font ), TextHAlign( HorizontalAlignment::Centre ), TextVAlign( VerticalAlignment::Centre )
{
	if( buttonclick == nullptr )
	{
		buttonclick = new RawSound( "STRATEGC/INTRFACE/BUTTON1.RAW" );
	}
}

void TextButton::EventOccured( Event* e )
{
	Control::EventOccured( e );

	// I was clicked, play a click sound
	if( e->Type == EVENT_FORM_INTERACTION && e->Data.Forms.RaisedBy == this && e->Data.Forms.EventFlag == FormEventType::MouseDown )
	{
		buttonclick->PlaySound();
	}
}

void TextButton::Render()
{
	Vector2* v = GetResolvedLocation();

	// PreRender();
	al_draw_filled_rectangle( v->X, v->Y, v->X + Size.X, v->Y + Size.Y, al_map_rgb( 128, 128, 128 ) );

	al_draw_line( v->X + 2, v->Y + 4, v->X + Size.X - 2, v->Y + 3, al_map_rgb( 220, 220, 220 ), 1 );
	al_draw_line( v->X + 2, v->Y + Size.Y - 4, v->X + Size.X - 2, v->Y + Size.Y - 4, al_map_rgb( 80, 80, 80 ), 1 );
	al_draw_line( v->X + 2, v->Y + Size.Y - 3, v->X + Size.X - 2, v->Y + Size.Y - 3, al_map_rgb( 64, 64, 64 ), 1 );
	al_draw_rectangle( v->X + 3, v->Y + 3, v->X + Size.X - 2, v->Y + Size.Y - 2, al_map_rgb( 64, 64, 64 ), 1 );

	if( mouseDepressed && mouseInside )
	{
		al_draw_rectangle( v->X + 1, v->Y + 1, v->X + Size.X - 1, v->Y + Size.Y - 1, al_map_rgb( 255, 255, 255 ), 2 );
	}
	
	int xpos;
	int ypos;

	switch( TextHAlign )
	{
		case HorizontalAlignment::Left:
			xpos = v->X;
			break;
		case HorizontalAlignment::Centre:
			xpos = v->X + (Size.X / 2) - (font->GetFontWidth( text ) / 2);
			break;
		case HorizontalAlignment::Right:
			xpos = v->X + Size.X - font->GetFontWidth( text );
			break;
	}

	switch( TextVAlign )
	{
		case VerticalAlignment::Top:
			ypos = v->Y;
			break;
		case VerticalAlignment::Centre:
			ypos = v->Y + (Size.Y / 2) - (font->GetFontHeight() / 2);
			break;
		case VerticalAlignment::Bottom:
			ypos = v->Y + Size.Y - font->GetFontHeight();
			break;
	}


	font->DrawString( xpos, ypos, text, APOCFONT_ALIGN_LEFT );
	delete v;

	PostRender();
}

void TextButton::Update()
{
	// No "updates"
}