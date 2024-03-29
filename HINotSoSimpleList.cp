/*
	File:		HINotSoSimpleList.cp

	Contains:	Demonstrates creating a not so simple list using the HIView APIs.

	Version:	1.1.1

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
	Boolean		inFocus;
	}
NotSoSimpleListData;

pascal OSStatus NSSViewHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);
CFStringRef GetNotSoSimpleListClass();

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
HICreateNotSoSimpleList(UInt32 value, UInt32 maximum, HIViewRef * outNotSoSimpleList)
	{
	HIViewRef scrollView;
	*outNotSoSimpleList = NULL;
	OSStatus status = HIScrollViewCreate(kHIScrollViewOptionsVertScroll | kHIScrollViewOptionsAllowGrow, &scrollView);
	if (status != noErr) goto exitCreate;

	HIObjectRef hiObject;
	status = HIObjectCreate(GetNotSoSimpleListClass(), 0, &hiObject);
	if (status != noErr) goto exitCreate;
	
	SetControl32BitMaximum((HIViewRef)hiObject, maximum);
	SetControl32BitMinimum((HIViewRef)hiObject, 1);
	SetControl32BitValue((HIViewRef)hiObject, value);
	
	status = HIViewAddSubview(scrollView, (HIViewRef)hiObject);
	if (status != noErr) goto exitCreate;

	HIViewSetVisible(scrollView, true);
	HIViewSetVisible((HIViewRef)hiObject, true);
  	*outNotSoSimpleList = scrollView;

exitCreate:
	return status;
	}

OSStatus NotSoSimpleListViewDrawing(CGContextRef context, const HIRect * bounds, const NotSoSimpleListData * myData)
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
	
	UInt32 max = GetControl32BitMaximum(myData->view);
	UInt32 val = GetControl32BitValue(myData->view) - 1;
	for (i = start; (i<= end) && (i < max); i++, y += myData->lineSize.height)
		{
		if (i == val)
			CGContextSetRGBFillColor(context, RGBToFloatRed(6), RGBToFloatGreen(6), RGBToFloatBlue(6), 1.0);
		else
			CGContextSetRGBFillColor(context, RGBToFloatRed(i % 6), RGBToFloatGreen(i % 6), RGBToFloatBlue(i % 6), 1.0);

		CGContextFillRect(context, lineBounds);

		if (i == val)
			CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);
		else
			CGContextSetRGBFillColor(context, 0.0, 0.0, 0.0, 1.0);

		CGContextSetTextPosition(context, 10.0, y + 0.75 * myData->lineSize.height);
		sprintf(p, "Line %ld", i+1);
		CGContextShowText(context, p, strlen(p));

		lineBounds.origin.y += myData->lineSize.height;
		}

	if (lineBounds.origin.y < bounds->size.height)
		{
		HIRect toErase = *bounds;
		toErase.origin = lineBounds.origin;
		toErase.size.height -= lineBounds.origin.y;
		CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);
		CGContextFillRect(context, toErase);
		}

	return noErr;
	}

/*----------------------------------------------------------------------------------------------------------*/
//	� GetNotSoSimpleListClass
//	Registers and returns an HIObject class for a not so simple list control.
/*----------------------------------------------------------------------------------------------------------*/
CFStringRef GetNotSoSimpleListClass()
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
				
				// the next 2 messages are NotSoSimpleList specific
			
				{ kEventNotSoSimpleList, kEventScrollableScrollThere },
				{ kEventNotSoSimpleList, kEventScrollableKeyScroll },
			
				// the next 2 messages are KeyBoard specific
			
				{ kEventClassKeyboard, kEventRawKeyDown },
				{ kEventClassKeyboard, kEventRawKeyRepeat },
			
				// the next 6 messages are Control specific

				{ kEventClassControl, kEventControlSetFocusPart },
				{ kEventClassControl, kEventControlGetFocusPart },
				{ kEventClassControl, kEventControlHitTest },
				{ kEventClassControl, kEventControlTrack },
				{ kEventClassControl, kEventControlValueFieldChanged },
				{ kEventClassControl, kEventControlGetPartRegion },
				{ kEventClassControl, kEventControlDraw }
			};
		
		HIObjectRegisterSubclass(kNotSoSimpleListClassID, kHIViewClassID, 0, NSSViewHandler,
								  GetEventTypeCount(kFactoryEvents), kFactoryEvents, 0, &theClass);
		}
	
	return kNotSoSimpleListClassID;
	}

enum
	{
	kControlListLinePart	= 172
	};

/*----------------------------------------------------------------------------------------------------------*/
//	� FindPart
//	returns which part the Point (from the EventRef) is in or kControlNoPart if not.
/*----------------------------------------------------------------------------------------------------------*/
ControlPartCode FindPart(EventRef inEvent, NotSoSimpleListData* myData, UInt32 * whichLine)
	{
	ControlPartCode part = kControlNoPart;

	UInt32 start = (int) (myData->originPoint.y / myData->lineSize.height);
	float y = start * myData->lineSize.height - myData->originPoint.y;

	// the point parameter is in view-local coords.
	HIPoint locPt;
	GetEventParameter(inEvent, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(locPt), NULL, &locPt);

	SInt32 line = 1 + (int) ((locPt.y - y - 1) / myData->lineSize.height);
	line += start;

	if (line < GetControl32BitMaximum(myData->view)) part = kControlListLinePart;
	
	if (whichLine != NULL) *whichLine = line;
	
	return part;
	}

float SanityCheck(float where, NotSoSimpleListData* myData)
	{
	// making we're not going off bounds
	HIRect bounds;
	HIViewGetBounds(myData->view, &bounds);

	HISize imageSize = myData->lineSize;
	imageSize.height *= GetControl32BitMaximum(myData->view);

	if (where + bounds.size.height > imageSize.height) 
		where = imageSize.height - bounds.size.height;
	if (where < 0) where = 0;
	
	return where;
	}

void ScrollToThere(float where, NotSoSimpleListData* myData)
	{
	// create the event and send it
	EventRef theEvent;
	CreateEvent(NULL, kEventNotSoSimpleList, kEventScrollableScrollThere, GetCurrentEventTime(), kEventAttributeUserEvent, &theEvent);
	HIPoint whereP = {0.0, where};
	SetEventParameter(theEvent, kEventParamOrigin, typeHIPoint, sizeof(whereP), &whereP);
	SendEventToEventTarget(theEvent, GetControlEventTarget(myData->view));
	ReleaseEvent(theEvent);
	}

/*----------------------------------------------------------------------------------------------------------*/
//	� NSSViewHandler
//	Event handler that implements our not so simple list control.
/*----------------------------------------------------------------------------------------------------------*/
pascal OSStatus NSSViewHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
	{
	OSStatus result = eventNotHandledErr;
	NotSoSimpleListData* myData = (NotSoSimpleListData*)inRefcon;

	switch (GetEventClass(inEvent))
		{
		case kEventClassHIObject:
			switch (GetEventKind(inEvent))
				{
				case kEventHIObjectConstruct:
					{
					// allocate some instance data
					NotSoSimpleListData* myData = (NotSoSimpleListData*) calloc(1, sizeof(NotSoSimpleListData));
					
					// get our superclass instance
					HIViewRef epView;
					GetEventParameter(inEvent, kEventParamHIObjectInstance, typeHIObjectRef, NULL, sizeof(epView), NULL, &epView);
					
					// remember our superclass in our instance data and initialize other fields
					myData->view = epView;
					myData->originPoint.x = 0;
					myData->originPoint.y = 0;
					myData->lineSize.width = 0;
					myData->lineSize.height = 24;
					myData->inFocus = false;
		
					// set the control ID so that we can find it later with HIViewFindByID
					result = SetControlID(myData->view, &kNotSoSimpleListViewID);
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
					imageSize.height *= GetControl32BitMaximum(myData->view);
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
					// we're being asked to scroll, we just do a sanity check and ask for a redraw if the location is different
					HIPoint where;
					GetEventParameter(inEvent, kEventParamOrigin, typeHIPoint, NULL, sizeof(where), NULL, &where);
					where.y = SanityCheck(where.y, myData);
					if (myData->originPoint.y == where.y) break;
					
					myData->originPoint.y = where.y;
					
					HIViewSetNeedsDisplay(myData->view, true);
					break;
					}
				
				default:
					break;
				}
			break;

		case kEventNotSoSimpleList:
			switch (GetEventKind(inEvent))
				{
				case kEventScrollableScrollThere:
					{
					// we're being asked to scroll, we just do a sanity check and ask for a redraw if the location is different
					HIPoint where;
					GetEventParameter(inEvent, kEventParamOrigin, typeHIPoint, NULL, sizeof(where), NULL, &where);
					where.y = SanityCheck(where.y, myData);
					if (myData->originPoint.y == where.y) break;
					
					myData->originPoint.y = where.y;
					
					// let's have the scroll bar reflect the scroll also
					EventRef theEvent;
					CreateEvent(NULL, kEventClassScrollable, kEventScrollableInfoChanged, GetCurrentEventTime(), kEventAttributeUserEvent, &theEvent);
					SendEventToEventTarget(theEvent, GetControlEventTarget(myData->view));
					ReleaseEvent(theEvent);
					
					HIViewSetNeedsDisplay(myData->view, true);
					break;
					}

				case kEventScrollableKeyScroll:
					{
					char theKey;
					GetEventParameter(inEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(theKey), NULL, &theKey);
					HIRect bounds;
					HIViewGetBounds(myData->view, &bounds);
					HISize imageSize = myData->lineSize;
					imageSize.height *= GetControl32BitMaximum(myData->view);

					float where = myData->originPoint.y;
					switch (theKey)
						{
						// the usual keys
						case kHomeCharCode: where = 0; break;
						case kEndCharCode: where = imageSize.height - bounds.size.height; break;
						case kPageUpCharCode: where -= (bounds.size.height - myData->lineSize.height); break;
						case kPageDownCharCode: where += (bounds.size.height - myData->lineSize.height); break;
						case kUpArrowCharCode: where -= myData->lineSize.height; break;
						case kDownArrowCharCode: where += myData->lineSize.height; break;
						// the rest
						default:
							if ((theKey >= 'a') && (theKey <= 'z'))
								// as a simple test we just go to the line number which is 10 times the "keystroke"
								where = (theKey - 'a') * 10 * myData->lineSize.height;
							break;
						}
					// sanity check
					where = SanityCheck(where, myData);
					if (where != myData->originPoint.y)
						// new location is different than previous so let's send a ScrollThere event
						ScrollToThere(where, myData);
					break;
					}
				
				default:
					break;
				}
			break;

		case kEventClassKeyboard:
			switch (GetEventKind(inEvent))
				{
				case kEventRawKeyDown:
				case kEventRawKeyRepeat:
					{
					char c;
					GetEventParameter(inEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(c), NULL, &c);

					switch (c)
						{
						default:
							// not within 'a-z', we bail out, else we test some other keys and send the event
							result = eventNotHandledErr;
							if ((c < 'a') || (c > 'z')) break;
						case kHomeCharCode:
						case kEndCharCode:
						case kPageUpCharCode:
						case kPageDownCharCode:
						case kUpArrowCharCode:
						case kDownArrowCharCode:
							{
							EventRef theEvent;
							CreateEvent(NULL, kEventNotSoSimpleList, kEventScrollableKeyScroll, GetCurrentEventTime(), kEventAttributeUserEvent, &theEvent);
							SetEventParameter(theEvent, kEventParamKeyMacCharCodes, typeChar, sizeof(c), &c);
							SendEventToEventTarget(theEvent, GetControlEventTarget(myData->view));
							ReleaseEvent(theEvent);
							result = noErr;
							}
							break;
						}
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
					NotSoSimpleListViewDrawing(context, &bounds, myData);

					CGContextRestoreGState(context);
					result = noErr;
					break;
					}

				case kEventControlHitTest:
					{
					ControlPartCode part = FindPart(inEvent, myData, NULL);

					SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(part), &part);

					result = noErr;
					break;
					}

				case kEventControlTrack:
					{
					UInt32 whichLine;
					ControlPartCode part = FindPart(inEvent, myData, &whichLine);

					// was part clicked hilited before? If not, hilite it.
					if (part == kControlListLinePart)
						SetControl32BitValue(myData->view, whichLine);

					HIViewRef contentView;
					HIViewFindByID(HIViewGetRoot(GetControlOwner(myData->view)), kHIViewWindowContentID, &contentView);
					HIRect theHIRect;
					HIViewGetFrame(myData->view, &theHIRect);
					HIViewConvertRect(&theHIRect, myData->view, contentView);

					// tracking (needs a QD Grafport. Actually it doesn't but it's simpler that way for managing local vs. global coords)
					GrafPtr savePort;
					GetPort(&savePort);
					SetPort(GetWindowPort(GetControlOwner(myData->view)));
					MouseTrackingResult mouse = kMouseTrackingMouseDown;
					for (; mouse != kMouseTrackingMouseUp; )
						{
						Point pt;
						TrackMouseLocationWithOptions(NULL, kTrackMouseLocationOptionDontConsumeMouseUp, TicksToEventTime(4), &pt, NULL, &mouse);
						if ((mouse == kMouseTrackingMouseDragged) || (mouse == kMouseTrackingTimedOut))
							{
							UInt32 start = (int) (myData->originPoint.y / myData->lineSize.height);
							float y = start * myData->lineSize.height - myData->originPoint.y;
							UInt32 line = 1 + (int) ((pt.v - theHIRect.origin.y - y - 1) / myData->lineSize.height);
							if (pt.v < theHIRect.origin.y) line--; // -5 divided by 20 is the same as 5 divided by 20!
							line += start;
							if (line != whichLine)
								{
								whichLine = line;
								SetControl32BitValue(myData->view, whichLine);
								
								// with auto scroll, we need to check if that just selected line is visible and if not, scroll there.
								SInt32 goThere = -1;
								if (whichLine <= start)
									goThere = whichLine - 1;
								else if (whichLine >= start + theHIRect.size.height / myData->lineSize.height)
									goThere = whichLine - (int)(theHIRect.size.height / myData->lineSize.height);
									
								if (goThere != -1)
									{
									float where = goThere * myData->lineSize.height;
									// sanity check
									where = SanityCheck(where, myData);
									if (where != myData->originPoint.y)
										// new location is different than previous so let's send a ScrollThere event
										ScrollToThere(where, myData);
									}
								}
							}
						}
					SetPort(savePort);

					SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(part), &part);
					result = noErr;
					break;
					}

				case kEventControlValueFieldChanged:
					{
					// sanity check
					myData->originPoint.y = SanityCheck(myData->originPoint.y, myData);
					
					EventRef theEvent;
					CreateEvent(NULL, kEventClassScrollable, kEventScrollableInfoChanged, GetCurrentEventTime(), kEventAttributeUserEvent, &theEvent);
					SendEventToEventTarget(theEvent, GetControlEventTarget(myData->view));
					ReleaseEvent(theEvent);
					HIViewSetNeedsDisplay(myData->view, true);
					break;
					}
				
				case kEventControlSetFocusPart:
					{
					ControlPartCode part;
					GetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, NULL, sizeof(part), NULL, &part);
					if (part == kControlFocusNoPart)
						// we lost focus
						myData->inFocus = false;
					else
						// we had focus, lose it, if we hadn't, get it.
						myData->inFocus = ! myData->inFocus;
					}
				case kEventControlGetFocusPart:
					{
					ControlPartCode part = (myData->inFocus)?1:0;
					SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(part), &part);
					result = noErr;
					}
					break;

				case kEventControlGetPartRegion:
					{
					ControlPartCode part;
					GetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, NULL, sizeof(part), NULL, &part);
					
					RgnHandle theRegion;
					GetEventParameter(inEvent, kEventParamControlRegion, typeQDRgnHandle, NULL, sizeof(theRegion), NULL, &theRegion);
					
					if ((part == kControlContentMetaPart) || (part == kControlStructureMetaPart) || (part == -3 /* kControlOpaqueMetaPart */))
						{
						HIRect bounds;
						HIViewGetBounds(myData->view, &bounds);
						
						Rect qdBounds;
						qdBounds.top    = (SInt16) CGRectGetMinY(bounds);
						qdBounds.left   = (SInt16) CGRectGetMinX(bounds);
						qdBounds.bottom = (SInt16) CGRectGetMaxY(bounds);
						qdBounds.right  = (SInt16) CGRectGetMaxX(bounds);
						
						RectRgn(theRegion, &qdBounds);
						}
					
					result = noErr;
					}
					break;
				
				default:
					break;
				}
			break;
			
		default:
			break;
		}
	
	return result;
	}
