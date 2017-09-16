/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>

    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

function Search(text)
{
	document.activeElement.value = text;
	document.getElementsByName("btnK").forEach(function(btn){btn.click();});
}
