/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>

    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

var keyword = ["beethoven", "classical", "symphony", "bach", "mozart", "nocture", "decca"];

window.setTimeout(()=>
{
	Google.Search(keyword[Math.floor(Math.random() * keyword.length)]);
}, 1000);
