// popupalert.js: Implementation file for Search Shield popup alert
//
// Copyright 2009 AVG Exploit Prevention Labs, Inc.
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


function HidePopupAlert(e)
{
	var div = document.getElementById("XPLSS_PopupAlert");
	if ((div == null) || (div.style == null) || (div.style.visibility == "hidden"))
		return;

	// make sure we aren't still over the popped image or our layer
	if (containedIn(div, e.relatedTarget) || (typeof(poppedElement) == "undefined"))
	{
		return;
	}
	// else hide the element

	// hide and move somewhere off screen (negative offsets)
	div.style.visibility = "hidden";
	div.style.left = -2100;
	div.style.top  = -2100;

	// get the url and redirect to it
	var popupurl = document.getElementById("avgalertpopurl");
	if (popupurl)
	{
		document.location = unescape(popupurl.innerHTML);
	}
}

function HidePopupAlertTimer(e, time)
{
	// init timer to 3 seconds the first time through
	if (time == null)
		time = 3;

	if (time > 0)
	{
		time--;
		setTimeout("HidePopupAlertTimer(null," + time + ")", 1000);
		return;
	}

	var div = document.getElementById("XPLSS_PopupAlert");
	if ((div == null) || (div.style == null) || (div.style.visibility == "hidden"))
		return;

	// hide and move somewhere off screen (negative offsets)
	div.style.visibility = "hidden";
	div.style.left = -2100;
	div.style.top  = -2100;

	// get the url and redirect to it
	var popupurl = document.getElementById("avgalertpopurl");
	if (popupurl)
	{
		document.location = unescape(popupurl.innerHTML);
	}
}

window.addEventListener("avgalertstart", HidePopupAlertTimer, false);
