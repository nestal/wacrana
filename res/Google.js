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
		console.log("google search for " + text);

		// assume the active element is the search text box
		// need to call blur otherwise click button doesn't work
		document.activeElement.value = text;
		document.activeElement.blur();

		window.setTimeout(() =>
		{
			document.activeElement.blur();
			document.getElementsByName("btnK").forEach(btn =>
			{
				btn.click();
			}), 1000
		});
//		window.location = "http://google.com/search?safe=active&q="+encodeURI(text);
	}

	static RelatedWords()
	{
		let result = [];
		document.querySelectorAll("div.card-section").forEach(node =>
		{
			result.push(node.innerText);
		});
		return result;
	}

	static SearchResults()
	{
		let result = [];
		document.querySelectorAll("h3 > a").forEach(node =>
		{
			console.log("href = " + node.href);
			result.push({
				text: node.textContent,
				href: node.href,
				desc: node.parentNode.nextSibling.textContent
			});
		});
		return result;
	}

	static IAmFeelingLucky()
	{
		let node = document.querySelector("h3 > a");
		if (node)
		{
			console.log("clicking = " + node.href);
			node.click();
		}
	}
}
