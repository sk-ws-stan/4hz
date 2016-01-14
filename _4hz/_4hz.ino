#include <FastLED.h>
// how fast to move the dots, in frames per second, adjust this to 
// make the dots move faster or slower
const unsigned short c_Speed = 100U;
// Change this to adjust how long of a tail a dot will have
const unsigned short c_FadeTail = 20U;
const unsigned short c_NumberOfLEDs = 100U;
const unsigned short c_Stripes = 4U;
const unsigned short c_Saturation = 63U;
const unsigned short c_Value = 83U;
const unsigned short c_ButtonOffset = 10U;
//debug flag
const boolean c_WriteToSerial = false;
//button status
boolean knopfGedryckt[ c_Stripes ];
//2d array for colors
CRGB leds[ c_Stripes ][ c_NumberOfLEDs ];

void setup()
{
  FastLED.addLeds< APA102, 1, 2 >( leds[ 0 ], c_NumberOfLEDs );
  FastLED.addLeds< APA102, 3, 4 >( leds[ 1 ], c_NumberOfLEDs );
  FastLED.addLeds< APA102, 5, 6 >( leds[ 2 ], c_NumberOfLEDs );
  FastLED.addLeds< APA102, 7, 8 >( leds[ 3 ], c_NumberOfLEDs );
  // Keep power usage sane (at least while testing)
  setMaxPowerInVoltsAndMilliamps( 5 /* V */ , 1000 /* mA */ );
  //initialise
  for( unsigned int i = 0U; i < c_Stripes; i++ )
  {
    for( unsigned int j = 0U; j < c_NumberOfLEDs; j++ )
    {
      leds[ i ][ j ] = CRGB( 0, 0, 0 );
    }
  }
  for( unsigned int i = 0U; i < c_Stripes; i++ )
  {
    knopfGedryckt[ i ] = false;
  }
}

void loop()
{ 
  //all LED stripes
  for( unsigned int i = 0U; i <= c_Stripes; i++ )
  {
    //seek for white dot from the back
    for( unsigned int j = c_NumberOfLEDs - 1U; j > 0U; j-- )
    {
      if( leds[ i ][ j ] == CRGB( 255, 255, 255 ) )
      {
        //last LED -> just ignore
        if( j != ( c_NumberOfLEDs - 1U ) )
        {
          //shift white one LED towards the back
          leds[ i ][ j + 1U ] = CRGB( 255, 255, 255 );
        }
        //fade out the colour to black from one after white up to c_FadeTail LEDs behind
        for( unsigned int fade = 0U; fade < c_FadeTail; fade++ )
        {
          //if not at start of strip
          if( ( j - 1U ) >= 0U )
          {
            CRGB current = leds[ i ][ j - 1U ];
            //check if another white dot
            if( current == CRGB( 255, 255, 255 ) )
            {
              //stop fading
              break;
            }
            //fade by argument (255 is max)
            current.fadeToBlackBy( ( 255 / c_FadeTail ) * fade );
            leds[ i ][ j ] = current;
          }
          //if at start of strip
          else
          {
            //stop fading
            break;
          }
        }
      }
      //else non white?
    }
    if( knopfGedryckt[ i ] )
    {
      //if a white dot was inserted
      if( leds[ i ][ 0 ] == CRGB( 255, 255, 255 ) )
      {
        //choose tail coulour and write
        CRGB colour;
        leds[ i ][ 0 ] = colour.setHSV( random( 255 ), c_Saturation, c_Value );
      }
      //need to fade the first LED if it's within a tail
      else if( leds[ i ][ 0 ] != CRGB( 0, 0, 0 ) )
      {
        leds[ i ][ 0 ].fadeToBlackBy( 255 / c_FadeTail );
      }
    }
    int currentKnopf = digitalRead( i + c_ButtonOffset );
    //first time button press
    if( ( currentKnopf == HIGH ) && ( knopfGedryckt[i] == false ) )
    {
      //set flag to handle one white per press
      knopfGedryckt[i] = true;
      //insert white dot at start
      leds[ i ][ 0 ] = CRGB( 255, 255, 255 );
    }
    //button was released
    else if( ( currentKnopf == LOW )  && ( knopfGedryckt[ i ] == true ) )
    {
      knopfGedryckt[ i ] = false;
    }
    //if pressed and flag set, nothing needs to be done

    //debug out to serial
    if( c_WriteToSerial )
    {
      Serial.print( leds[ i ][ 0 ] );
      Serial.print( "\t" );
      //carriage return
      if( i == ( c_Stripes - 1U ) )
      {
        Serial.println( "" );
      }
    }
  }
  //flush buffer
  FastLED.show();
}
