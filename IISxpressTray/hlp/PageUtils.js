function PageHeader(HeaderString)
{
document.write(
"<div class='mypageheader'>" +
"<span style='vertical-align: middle; width: 64px; height: 64px; background-image: url(/images/compass_64_24_cornflowerblue.png); background-repeat: no-repeat;'></span>" +
"<span style='margin-left: 12px;'>" + HeaderString + "</span>" +
"</div>" +
"<br/>"
);
}

function PageFooter()
{
document.write(
"<hr/>" +
"<span class='copyright'>&copy; 2005-2010 Ripcord Software. All rights reserved.</span>"
);
}