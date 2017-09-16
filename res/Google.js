/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>

    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

class Google
{
	static Search(text)
	{
		// assume the active element is the search text box
		// need to call blur otherwise click button doesn't work
		document.activeElement.value = text;
		document.activeElement.blur();

		window.setTimeout(() => document.getElementsByName("btnK").forEach(btn =>
		{
			btn.click();
		}), 1000);
	}
}
