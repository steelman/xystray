var pr='mailto:';
var nm='stlman';
var dm='poczta.fm';

function showLink() {
  document.write('<link rel="author" href="' +  pr + nm +"@" + dm + '">');
}

function showEmail() {
	var link=document.createElement("a")
	var mymail=document.getElementById('myemail');
	var text=mymail.firstChild;

	mymail.removeChild(text);
	link.setAttribute("href", pr+nm+'@'+dm);

	link.appendChild(text);
	mymail.appendChild(link);

	//item.innerHTML = '<a href="'+ pr + nm +"@" + dm + '">'+text+'</a>';
}
