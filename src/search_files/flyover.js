// flyover.js: Implementation file for SearchShield flyovers.
//
// Copyright 2006-2007 Exploit Prevention Labs, Inc.

function ShowFlyover()
{
	var div = document.getElementById("XPLSS_Flyover");
	if (div == null)
		return;

	div.style.visibility = "visible";

	var trans_div = document.getElementById("XPLSS_Trans");
	if (trans_div == null)
		return;
	trans_div.style.visible = "visible";
}

function containedIn(container, element)
{
	while (element != null) 
	{
		if (container == element) 
		{
			return true;
		}
		element = element.parentNode;
	}
	return false;
}

function HideFlyover(event)
{
	var div = document.getElementById("XPLSS_Flyover");
	if ((div == null) || (div.style == null) || (div.style.visibility == "hidden"))
		return;

	var trans_div = document.getElementById("XPLSS_Trans");
	if ((trans_div == null) || (trans_div.style == null) || (trans_div.style.visibility == "hidden"))
		return;

	// hide the flyover when the search term changes
	if ((event.type == "keydown") && (event.target instanceof HTMLInputElement))
	{
		setFlyoverStyle();
		return;
	}

	var relatedElement = event.relatedTarget;  

	// make sure we aren't still over the popped image or our layer
	if (containedIn(div, relatedElement) || (typeof(poppedElement) == "undefined")	||
		containedIn(poppedElement, relatedElement) || containedIn(trans_div, relatedElement))
	{
		return;
	}
	// This can happen when verdict is on a different frame from the flyover
	if ((relatedElement == null) && 
		(event.type == "mouseout") &&
		((event.originalTarget == poppedElement) || (event.originalTarget instanceof HTMLIFrameElement)))
	{
		return;
	}

	setFlyoverStyle();

	// hide and move somewhere off screen (negative offsets)
	function setFlyoverStyle()
	{
		div.style.visibility = "hidden";
		div.style.left = "-2100px";
		div.style.top  = "-2100px";
		trans_div.style.visibility = "hidden";
		trans_div.style.left = "-2100px";
		trans_div.style.top  = "-2100px";

		return true;
	}
}


var poppedUp = false;
var poppedElement = null;

function PopupFlyover(event, hash, search, flyover)
{
	var div = document.getElementById("XPLSS_Flyover");
	if (div == null)
		return;

	var eventTarget = event.currentTarget;

	// if the element is the clock, don't pop over it
	if ((eventTarget.src != null) && (eventTarget.src.indexOf("clock.gif") != -1))
		return;
	
	// save the element we popped over
	poppedElement = eventTarget;

	// if no flyover get it
	if ((flyover == null) || (flyover == ""))
	{
		// get flyover data element, used to pass data
		var element = document.getElementById("XplFlyoverDataElement");
		if (element == null)
		{
			element = document.createElement("xplflyoverdata");
			element.setAttribute("id", "XplFlyoverDataElement");
			document.documentElement.appendChild(element);
		}

		// set search and hash
		element.setAttribute("hash", hash);
		element.setAttribute("search", search);
		
		doEvent(event, element, "xplflyover", "mouseover", true, false);

		flyover = element.getAttribute("flyover");
		if (flyover == null)
			return;

		// cleanup flyover, replace any new lines
		flyover = flyover.replace(/\r/g, "");
		flyover = flyover.replace(/\n/g, "");
		// escape any single quotes
		flyover = flyover.replace(/'/g, "&#39;");

		if (poppedElement != null)
			poppedElement.setAttribute("onmouseover", "top.PopupFlyover(event,'" + hash + "','" + search + "','" + flyover + "')");
	}

	// set the html in the layer
	div.innerHTML = flyover;

	// there is an unwanted text node that cause misalignment of flyover
	if (div.firstChild.nodeType == Node.TEXT_NODE)
		div.removeChild(div.firstChild);

	poppedUp = true;

	div.onmouseout = HideFlyover;

	// in a frame
	if ((poppedElement) && (poppedElement.ownerDocument != document))
	{
		RemoveListener(poppedElement.ownerDocument, 'mousemove', MoveFlyover, false);
		AddListener(poppedElement.ownerDocument, 'mousemove', MoveFlyover, false);
	}
}

function GetFullBoundingRect(element)
{
	if (!element) return;

	// get bounding rect for incoming element
	var elementRect = element.getBoundingClientRect();
	var nextImg = null;
	var nextImgRect = null;

	// first check for another element after this one
	if (element && element.parentNode && 
		element.parentNode.nextSibling &&
		element.parentNode.nextSibling.firstChild && 
		element.parentNode.nextSibling.id && 
		element.parentNode.nextSibling.id.indexOf("XPLSS_"))
	{
		nextImg = element.parentNode.nextSibling.firstChild;		
		nextImgRect = nextImg.getBoundingClientRect();	
	}
	else
	{
		return elementRect;
	}
	
	if ((nextImgRect.top >= elementRect.bottom) && 
		(nextImgRect.left <= elementRect.left))
	{	// images appear to be on seperate lines
		return elementRect;
	}
	
	// else merge the rects together into a new one
	var newRect = new function() { this.top=0; this.left=0; this.right=0; this.bottom=0; this.mid=0;};
	newRect.top = Math.min(elementRect.top, nextImgRect.top);
	newRect.left= Math.min(elementRect.left, nextImgRect.left);
	newRect.right=Math.max(elementRect.right, nextImgRect.right);
	newRect.bottom=Math.max(elementRect.bottom, nextImgRect.bottom);
	newRect.mid = Math.min(elementRect.right, nextImgRect.left);
	
	return newRect;
}

function MoveFlyover(e)
{
	if (!poppedUp || (poppedElement == null))
		return;

	var flyover = document.getElementById("XPLSS_Flyover");
	if (flyover == null)
		return;

	// relative position of flyover in relation to icon
	var locateX = 0;  // 0=left, 1=right
	var locateY = 0;  // 0=above, 1=below, 2=beside icon

	var scrollXWidth = 19;  // approx
	
	// Must know if there is a horizontal scroll bar for Firefox
	// for proper flyover positioning near bottom edge
	var scrollBarX = false;	//default for Microsoft IE
	var scrollYWidth = 18;	//normally 17 (+1 top border)
	if (window.innerHeight)
	{	// not MSIE
		try
		{
			scrollYWidth = Math.floor(Math.abs(window.innerHeight - document.documentElement.clientHeight)) + 1;
			scrollBarX = (document.documentElement.clientWidth < document.documentElement.scrollWidth);
		}
		catch(err){}
	}

	// get window sizes
	if (window.innerHeight == undefined)	// Microsoft IE
	{
		var windowX = document.documentElement.clientWidth - scrollXWidth;
		var windowY = document.documentElement.clientHeight;
	}
	else
	{
		var windowX = window.innerWidth - scrollXWidth;
		var windowY = window.innerHeight;
		if(scrollBarX)
			windowY -= scrollYWidth;
	}

	// get the exact size of the flyover
	if (window.getComputedStyle == undefined)	// Microsoft IE
	{
		var flyoverX = parseInt(flyover.offsetWidth,10);
		var flyoverY = parseInt(flyover.offsetHeight,10);
	}
	else
	{
		var style = document.defaultView.getComputedStyle(flyover, "");
		var flyoverX = parseInt(style.width,10);
		var flyoverY = parseInt(style.height,10);
	}

	var verdictWidth = 0;
	if (poppedElement && poppedElement.width)
		verdictWidth = poppedElement.width;
	
	// adjustment so trans div doesn't overlap entire verdict icon
	// to allow click_thru to work
	if (poppedElement.parentNode.href && poppedElement.parentNode.href != "javascript:void(0)")
		verdictWidth = 2;
	
	// get the bounding rect for image(s)
	var imgRect = GetFullBoundingRect(poppedElement);

	// half width/height of element bounding rect
	var halfX = (imgRect.right - imgRect.left) / 2;
	var halfY = (imgRect.bottom- imgRect.top) / 2;

	// element the mouse is over, get the center position
	var posX = offsetLeft(poppedElement) + halfX;
	var posY = offsetTop(poppedElement) + halfY;

	var winFrames = window.frames;
	for (var i=0; i < winFrames.length; i++)
	{
		// some frames may be inaccessible (different domain)
		try {
			if (winFrames[i].frameElement.contentDocument.getElementById(poppedElement.id))
			{
				posX += offsetLeft(winFrames[i].frameElement);
				posY += offsetTop(winFrames[i].frameElement);
			}
		} catch(fErr){}
	}

	var transXOffset = 0;
	if (imgRect.mid == undefined)
		transXOffset = -1 * halfX;
	else
		transXOffset = ((imgRect.right + imgRect.left) / 2) - imgRect.mid;


	var pageOffsetX = 0;
	var pageOffsetY = 0;

	// normalize pos to 0  -- get amount of scrolling in browser window
	if (window.pageXOffset == undefined)	// Microsoft IE
	{
		pageOffsetX = document.documentElement.scrollLeft;
		pageOffsetY = document.documentElement.scrollTop;
	}
	else
	{
		pageOffsetX = window.pageXOffset;
		pageOffsetY = window.pageYOffset;
	}

	posX -= pageOffsetX;
	posY -= pageOffsetY;

	// setup the offsets
	var offsetX = posX;
	var offsetY = posY;

	// calc where to display on page
	if ((windowX - posX) > posX)
	{
		// right
		offsetX += halfX;
		locateX = 1;
	}
	else
	{
		//left
		offsetX -= (flyoverX + halfX);
	}
	if ((windowY - posY) > posY)
	{
		// below
		if (posY < (windowY/4))
		{
			offsetY -= halfY;
			locateY = 1;
		}
		else
		{
			offsetY -= (flyoverY / 2);
			locateY = 2;
		}
	}
	else
	{
		// above
		if ((windowY - posY) < (windowY/4))
		{
			offsetY -= (flyoverY - halfY);
		}
		else
		{
			offsetY -= (flyoverY / 2);
			locateY = 2;
		}
	}
	// make sure we aren't off the screen
	if (offsetY < 0)
		offsetY = 0;

	if ((offsetY + flyoverY) > windowY)
		offsetY = windowY - flyoverY;

	// add page offsets back
	offsetX += pageOffsetX;
	offsetY += pageOffsetY;
	posX += pageOffsetX;
	posY += pageOffsetY;

	var paddedOffsetX = 0; //provide space between icon and flyover
	var padX = 3;
	if (locateX == 0)
		paddedOffsetX = offsetX - padX;
	else
		paddedOffsetX = offsetX + padX;


	// set where to put the flyover
	flyover.style.top = offsetY + "px";
	flyover.style.left = paddedOffsetX + "px";


	// set where to put the transparent layer
	var trans = document.getElementById("XPLSS_Trans");
	if (trans != null)
	{
		var trans_left = 0;
		var trans_top  = 0;
		var trans_width= 0;
		var trans_height = 0;

		// transparent layer should overlap verdict image
		if (locateX == 0)
			trans_left = posX - flyoverX - halfX;  // left 
		else
			trans_left = posX - transXOffset - verdictWidth; // right

		trans.style.left	= trans_left + "px";
		trans.style.top		= offsetY + "px";

		trans.style.width	= flyoverX + poppedElement.width + "px";
		trans.style.height	= flyoverY + "px";
	}

	DisplayFlyover();
}


function DisplayFlyover()
{
	if (poppedElement == null)
		return;

	// Make sure the element is still in the document
	var elemDoc = poppedElement.ownerDocument;
	if (!elemDoc.getElementById(poppedElement.id))
		return;

	// Make sure the associated parent node is visible - for google instant
	var elemAnchor = poppedElement.parentNode;
	if (window.getComputedStyle(elemAnchor.parentNode, "").visibility != "visible")
		return;

	// show the flyover
	var flyover = document.getElementById("XPLSS_Flyover");
	if (flyover == null)
		return;
	flyover.style.visibility = "visible";

	// show the transparent layer
	var trans_div = document.getElementById("XPLSS_Trans");
	if (trans_div == null)
		return;
	trans_div.style.visibility = "visible";

	poppedUp = false;
}

function offsetTop(element)
{
	var offset = 0;
	while (element)
	{
		offset += element.offsetTop;	
		element = element.offsetParent;
	}

	return offset;
}

function offsetLeft(element)
{
	var offset = 0;
	while (element)
	{
		offset += element.offsetLeft;
		element = element.offsetParent;
	}

	return offset;
}

////// GENERAL UTILITY FUNCTIONS //////
function AddListener(object, evtType, listener, useCapture)
{
	useCapture = !!useCapture;
	
	if (window.addEventListener)
	{
		object.addEventListener(evtType, listener, useCapture);
		return true;
	}
	else if (window.attachEvent)
	{
		 object.attachEvent("on"+evtType, listener);
		 return true;
	}
	
	return false;
}

function RemoveListener(object, evtType, listener, useCapture)
{
	useCapture = !!useCapture;
	
	if (window.removeEventListener)
	{
		object.removeEventListener(evtType, listener, useCapture);
		return true;
	}
	else if (window.detachEvent)
	{
		 object.detachEvent(evtType, listener);
		 return true;
	}
	
	return false;
}

function doEvent(evtObj, evtTarget, evtName, evtType, bubbles, cancelable)
{
	bubbles		= !!bubbles;
	cancelable	= !!cancelable;
	
	if (document.createEvent)
	{
		var evt = document.createEvent("Events");
		evt.initEvent(evtName, bubbles, cancelable);
		evtTarget.dispatchEvent(evt);
		return true;
	}
	else if (document.createEventObject)
	{
		var evt = document.createEventObject(evtObj);
		evtTarget.fireEvent("on" + evtType, evt);
		return true;
	}
	
	return false;
}

AddListener(window, "DOMMouseScroll", HideFlyover, false);
AddListener(document, "mouseout", HideFlyover, false);
AddListener(document, "keydown", HideFlyover, false);
AddListener(document, "mousemove", MoveFlyover, false);
