/*
	File:		HISimpleList.cp

	Contains:	Demonstrates creating a simple list using the HIView APIs.

	Version:	1.0.2

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

#include <Carbon/Carbon.h>
#include "HISimpleList.h"

// structure in which we hold our custom push button's data
typedef struct
	{
	HIViewRef	view;							// the HIViewRef for our list
	HIPoint		originPoint;
	HISize		lineSize;
	UInt32		nbLines;
	}
SimpleListData;

pascal OSStatus ViewHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);
CFStringRef GetSimpleListClass();

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
HICreateSimpleList(HIViewRef * outSimpleList)
	{
	HIViewRef scrollView;
	*outSimpleList = NULL;
	OSStatus status = HIScrollViewCreate(kHIScrollViewOptionsVertScroll | kHIScrollViewOptionsAllowGrow, &scrollView);
	if (status != noErr) goto exitCreate;

	HIObjectRef hiObject;
	status = HIObjectCreate(GetSimpleListClass(), 0, &hiObject);
	if (status != noErr) goto exitCreate;
	
	status = HIViewAddSubview(scrollView, (HIViewRef)hiObject);
	if (status != noErr) goto exitCreate;

	HIViewSetVisible(scrollView, true);
	HIViewSetVisible((HIViewRef)hiObject, true);
//	HIScrollViewSetScrollBarAutoHide(scrollView, true);
  	*outSimpleList = scrollView;

exitCreate:
	return status;
	}

// background colors for alternate lines

#define kWhitish 57536

RGBColor gBackGrounds[] =
	{
		{65535, kWhitish, kWhitish},
		{kWhitish, 65535, kWhitish},
		{kWhitish, kWhitish, 65535},
		{65535, 65535, kWhitish},
		{65535, kWhitish, 65535},
		{kWhitish, 65535, 65535},
		{3060, 3060, 30600}
	};

OSStatus SimpleListViewDrawing(CGContextRef context, const HIRect * bounds, const SimpleListData * myData)
	{
	UInt32 start, end, i;
	start = (int) (myData->originPoint.y / myData->lineSize.height);
	end = (int) ((myData->originPoint.y + bounds->size.height) / myData->lineSize.height + 1);

	CGContextSelectFont(context, "Helvetica", myData->lineSize.height * 0.75, kCGEncodingMacRoman);
	char p[20];
	HIRect lineBounds = *bounds;
	lineBounds.size.height = myData->lineSize.height;
	float y = start * myData->lineSize.height - myData->originPoint.y;
	lineBounds.origin.y = y;
	for (i = start; (i<= end) && (i < myData->nbLines); i++, y += myData->lineSize.height)
		{
		CGContextSetRGBFillColor(context, RGBToFloatRed(i % 6), RGBToFloatGreen(i % 6), RGBToFloatBlue(i % 6), 1.0);
		CGContextFillRect(context, lineBounds);
		CGContextSetRGBFillColor(context, 0.0, 0.0, 0.0, 1.0);

		CGContextSetTextPosition(context, 10.0, y + 0.75 * myData->lineSize.height);
		sprintf(p, "Line %ld", i+1);
		CGContextShowText(context, p, strlen(p));

		lineBounds.origin.y += myData->lineSize.height;
		}
	return noErr;
	}

/*----------------------------------------------------------------------------------------------------------*/
//	� GetSimpleListClass
//	Registers and returns an HIObject class for a simple list control.
/*----------------------------------------------------------------------------------------------------------*/
CFStringRef GetSimpleListClass()
	{
	
	// following code is pretty much boiler plate.
	
	static HIObjectClassRef	theClass;
	
	if (theClass == NULL)
		{
		static EventTypeSpec kFactoryEvents[] =
			{
				// the next 3 messages are boiler plate
				
				{ kEventClassHIObject, kEventHIObjectConstruct },
				{ kEventClassHIObject, kEventHIObjectInitialize },
				{ kEventClassHIObject, kEventHIObjectDestruct },
			
				// the next 2 messages are Scroll specific
			
				{ kEventClassScrollable, kEventScrollableGetInfo },
				{ kEventClassScrollable, kEventScrollableScrollTo },
			
				// the next 2 messages are Control specific
			
				{ kEventClassControl, kEventControlSetData },
				{ kEventClassControl, kEventControlDraw }
			};
		
		HIObjectRegisterSubclass(kSimpleListClassID, kHIViewClassID, 0, ViewHandler,
								  GetEventTypeCount(kFactoryEvents), kFactoryEvents, 0, &theClass);
		}
	
	return kSimpleListClassID;
	}

/*----------------------------------------------------------------------------------------------------------*/
//	� ViewHandler
//	Event handler that implements our simple list control.
/*----------------------------------------------------------------------------------------------------------*/
pascal OSStatus ViewHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
	{
	OSStatus result = eventNotHandledErr;
	SimpleListData* myData = (SimpleListData*)inRefcon;

	switch (GetEventClass(inEvent))
		{

		case kEventClassHIObject:
			switch (GetEventKind(inEvent))
				{
				case kEventHIObjectConstruct:
					{
					// allocate some instance data
					SimpleListData* myData = (SimpleListData*) calloc(1, sizeof(SimpleListData));
					
					// get our superclass instance
					HIViewRef epView;
					GetEventParameter(inEvent, kEventParamHIObjectInstance, typeHIObjectRef, NULL, sizeof(epView), NULL, &epView);
					
					// remember our superclass in our instance data and initialize other fields
					myData->view = epView;
					myData->originPoint.x = 0;
					myData->originPoint.y = 0;
					myData->lineSize.width = 0;
					myData->lineSize.height = 24;
					myData->nbLines = 100;
		
					// set the control ID so that we can find it later with HIViewFindByID
					result = SetControlID(myData->view, &kSimpleListViewID);
					if (result != noErr) DebugStr("\pSetControlID failed!");
					
					// store our instance data into the event
					result = SetEventParameter(inEvent, kEventParamHIObjectInstance, typeVoidPtr, sizeof(myData), &myData);

					break;
					}
					
				case kEventHIObjectDestruct:
					{
					free(myData);
					result = noErr;
					break;
					}

				case kEventHIObjectInitialize:
					{
					// always begin kEventHIObjectInitialize by calling through to the previous handler
					result = CallNextEventHandler(inCaller, inEvent);
					
					// if that succeeded, do our own initialization
					if (result == noErr)
						{
						// nothing to do here
						}
					break;
					}
				
				default:
					break;
				}
			break;

		case kEventClassScrollable:
			switch (GetEventKind(inEvent))
				{
				case kEventScrollableGetInfo:
					{
					// we're being asked to return information about the scrolled view that we set as Event Parameters
					HISize imageSize = myData->lineSize;
					imageSize.height *= myData->nbLines;
					SetEventParameter(inEvent, kEventParamImageSize, typeHISize, sizeof(imageSize), &imageSize);
					HIRect bounds;
					HIViewGetBounds(myData->view, &bounds);
					SetEventParameter(inEvent, kEventParamViewSize, typeHISize, sizeof(bounds.size), &bounds.size);
					SetEventParameter(inEvent, kEventParamLineSize, typeHISize, sizeof(myData->lineSize), &myData->lineSize);
					SetEventParameter(inEvent, kEventParamOrigin, typeHIPoint, sizeof(myData->originPoint), &myData->originPoint);
					result = noErr;
					break;
					}

				case kEventScrollableScrollTo:
					{
					// we're being asked to scroll, we just do a sanity check and ask for a redraw
					HIPoint where;
					GetEventParameter(inEvent, kEventParamOrigin, typeHIPoint, NULL, sizeof(where), NULL, &where);
					myData->originPoint.y = (where.y < 0.0)?0.0:where.y;
					HIViewSetNeedsDisplay(myData->view, true);
					break;
					}
				
				default:
					break;
				}
			break;

		case kEventClassControl:
			switch (GetEventKind(inEvent))
				{

				//	Draw the view.

				case kEventControlDraw:
					{
					CGContextRef context;
					result = GetEventParameter(inEvent, kEventParamCGContextRef, typeCGContextRef, NULL, sizeof(context), NULL, &context);
					if (result != noErr) {DebugStr("\pGetEventParameter failed for kEventControlDraw"); break;}

					HIRect bounds;
					HIViewGetBounds(myData->view, &bounds);
					
					CGContextSaveGState(context);
					CGAffineTransform transform = CGAffineTransformIdentity;
					// adjust the transform so the text doesn't draw upside down
					transform = CGAffineTransformScale(transform, 1, -1);
					CGContextSetTextMatrix(context, transform);
					
					// now that the proper parameters and configurations have been dealt with, let's draw
					SimpleListViewDrawing(context, &bounds, myData);

					CGContextRestoreGState(context);
					result = noErr;
					break;
					}

				case kEventControlSetData:
					{
					// we override this event, being only interested in the kSetNumberOfLinesCommand tag
					// if that's the case, we then get the value, and send a kEventClassScrollable + kEventScrollableInfoChanged
					// to the scrollview so that it will then ask us for a kEventClassScrollable + kEventScrollableGetInfo and change
					// its internal state, we also ask for a later redraw.
					
					UInt32 theCommand, theData, theNewValue;
					Boolean changed = false;
					GetEventParameter(inEvent, kEventParamControlDataTag, typeEnumeration, NULL, sizeof(theCommand), NULL, &theCommand);
					GetEventParameter(inEvent, kEventParamControlDataBuffer, typePtr, NULL, sizeof(theData), NULL, &theData);
					
					switch (theCommand)
						{
						case kSetNumberOfLinesCommand:
							{
							theNewValue = *((UInt32 *)theData);
							if (theNewValue != myData->nbLines) {changed = true; myData->nbLines = theNewValue;}
							}
							break;

						default:
							break;
						}
					
					if (changed)
						{
						HISize imageSize = myData->lineSize;
						imageSize.height *= myData->nbLines;
						HIRect bounds;
						HIViewGetBounds(myData->view, &bounds);
						if (myData->originPoint.y + bounds.size.height > imageSize.height) 
							myData->originPoint.y = imageSize.height - bounds.size.height;
						if (myData->originPoint.y < 0) myData->originPoint.y = 0;
						EventRef theEvent;
						CreateEvent(NULL, kEventClassScrollable, kEventScrollableInfoChanged, GetCurrentEventTime(), kEventAttributeUserEvent, &theEvent);
						SendEventToEventTarget(theEvent, GetControlEventTarget(myData->view));
						ReleaseEvent(theEvent);
						HIViewSetNeedsDisplay(myData->view, true);
						HIViewSetNeedsDisplay(HIViewGetSuperview(myData->view), true);
						}
					break;
					}
				
				default:
					break;
				}
			break;
			
		default:
			break;
		}
	
	return result;
	}
