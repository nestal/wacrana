/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>

    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

function Beethoven(keywords)
{
	window.setTimeout(() => {
		console.log("Beethoven is searching for " + keywords);
		Google.Search(keywords);
	}, 1000);
}
