/*
	File:		HISimpleList.h

	Contains:	Header file associated with HISimpleList.cp.

	Version:	1.0

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Copyright � 2002 Apple Computer, Inc., All Rights Reserved
*/

#ifndef __HISIMPLELIST__
#define __HISIMPLELIST__

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
	{
	kSetNumberOfLinesCommand = 'SNLC'
	};

extern RGBColor gBackGrounds[];

#define RGBToFloatRed(x) ((float)gBackGrounds[x].red) / 65535
#define RGBToFloatGreen(x) ((float)gBackGrounds[x].green) / 65535
#define RGBToFloatBlue(x) ((float)gBackGrounds[x].blue) / 65535

const ControlID kSimpleListViewID = {'CASD', 'HSLV'};

#define kSimpleListClassID	CFSTR("com.apple.sample.dts.HISimpleList")

/*
 *  HICreateSimpleList()
 *  
 *  Summary:
 *    Creates a simple list control.
 *  
 *  Parameters:
 *    
 *    outSimpleList:
 *      On exit, contains the new control.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.2 and later since it needs the HIView APIs
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   not available
 */

extern OSStatus 
HICreateSimpleList(HIViewRef * outSimpleList);

const ControlID kNotSoSimpleListViewID = {'CASD', 'HNLV'};

#define kEventNotSoSimpleList 'NSSL'

#define kEventScrollableKeyScroll 256
#define kEventScrollableScrollThere 257

#define kNotSoSimpleListClassID	CFSTR("com.apple.sample.dts.HINotSoSimpleList")

/*
 *  HINotSoCreateSimpleList()
 *  
 *  Summary:
 *    Creates a not so simple list control.
 *  
 *  Parameters:
 *    
 *    value:
 *      The currently selected line of our not so simple list.
 *    
 *    maximum:
 *      The number of lines of our not so simple list.
 *    
 *    outNotSoSimpleList:
 *      On exit, contains the new control.
 *  
 *  Availability:
 *    Mac OS X:         in version 10.2 and later since it needs the HIView APIs
 *    CarbonLib:        not available
 *    Non-Carbon CFM:   not available
 */

extern OSStatus 
HICreateNotSoSimpleList(UInt32 value, UInt32 maximum, HIViewRef * outNotSoSimpleList);

#ifdef __cplusplus
}
#endif

#endif
