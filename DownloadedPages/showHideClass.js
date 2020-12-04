function showHideClass(className) {
	let elems = document.getElementsByClassName(className);
	if (elems === null) {
		return;
	}
	for (let i = 0; i < elems.length; ++i) {
		let displayProp = elems[i].style.display;
	if (displayProp === 'none')
		elems[i].style['display'] = "inline";
	else
		elems[i].style['display'] = "none";
	}
}
