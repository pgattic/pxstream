"use strict"; // Using strict mode. Like a giga-chad.

// global stuff

const $=(x)=>{return document.querySelector(x)}

const imageScale = 4;

async function retrieve(url) {
	const response = await fetch(url);
	return await response.json();
}

let
	out = $("#output"),
	ctx = out.getContext("2d");


// Event listeners and their related functions

function readFileContent(file) {
	const reader = new FileReader();
	return new Promise((resolve, reject) => {
		reader.onload = event => resolve(event.target.result);
		reader.onerror = error => reject(error);
		reader.readAsText(file);
	});
}

function placeFileContent(file) {
	readFileContent(file).then(content => {
		ctx.clearRect(0, 0, out.width, out.height);
		clearInterval(go);
		let stream = JSON.parse(content);
		$('#limit').style.display = "initial";
		$('#limit').max = stream[1].length;
		$("#limit").value = stream[1].length;
		$("#output").width = stream[0][0] * imageScale;
		$("#output").height = stream[0][1] * imageScale;
		displayImg(stream, stream[1].length);
	}).catch(error => console.log(error))
}

upload.onchange = (e) => {
	const input = e.target;
	if ('files' in input && input.files.length > 0) {
		placeFileContent(input.files[0]);
	}
}

$("#limit").oninput = () => {
	displayImg(storeResult, Number($('#limit').value));
	$('#bytes').innerText=$('#limit').value;	
}


// The rest of this code is admittedly kind of messy, but calcPos() is where the fun math happens

function calcPos(num) { // calculates x, y, and width all from one integer!
//	let b4 = num.toString(4); // convert to base 4
	let base4digs = Math.floor(Math.log2(num) / 2) + 1; // sigfigs of num but in base-4 notation
	let x = 0, y = 0, w = 2 ** (-base4digs);
	for (let i = 0; i < base4digs; i++) { // the secret to the algorithm: the least significant digit influences the pixel's location the most.
//		let newVal = b4[b4.length-i];
		let channels = (num & (3 << (i * 2))) >> (i * 2);
		let inc = 2 ** (-i - 1);
		switch (channels) {
			case 0:
				break;
			case 3:
				x += inc;
				break;
			case 2:
				y += inc;
				break;
			case 1:
				x += inc;
				y += inc;
				break;
		}
	}
	return [x, y, w];
}

function doPixel(result, currPix, renderLimit=16384, numOfSkipped, multiple) {
	let coords;
	do {
		coords = calcPos(currPix + numOfSkipped);
		numOfSkipped++;
	} while (coords[0]*renderLimit >= result[0][0] || coords[1]* renderLimit >= result[0][1])
	numOfSkipped--;
	let [x, y, w] = coords;
	let boxOffset = -(imageScale * renderLimit * w/2);
	ctx.beginPath();
	ctx.rect(x*multiple + boxOffset, y*multiple + boxOffset, w * multiple, w * multiple);
//	ctx.rect(x*out.width, y*out.height, out.width/128, out.height/128); // replace previous line with this to see the image without interpolation
	ctx.fillStyle = result[1][currPix];
	ctx.fill();
	return numOfSkipped;
}


let storeResult;

let displayImg = (result = [...storeResult], limit=16384) => {
	storeResult = [...result];
	let renderLimit = 2 ** Math.ceil(Math.log2(Math.max(result[0][0], result[0][1]))); // dimension of image if rounded up to nearest power of 2
	let numOfSkipped = 0;
	let multiple = renderLimit * imageScale;
	// adjust image origin to remove white edges
	ctx.setTransform(1, 0, 0, 1, 0, 0);
	ctx.clearRect(0, 0, out.width, out.height);
	let translateDist = imageScale / 2;
	ctx.setTransform(1, 0, 0, 1, translateDist, translateDist);
	for (let currPix = 0; currPix < limit; currPix++) {
		numOfSkipped = doPixel(result, currPix, renderLimit, numOfSkipped, multiple);
	}
	$("#bytes").innerText = limit;
}


let p = 0;
let pIncrement = 1;
let pixelSize = 1;
let numOfPixels = Math.pow((512 / pixelSize), 2);

let go = setInterval(()=>{ // "Algorithm demo", plays upon load
	let [x, y, w] = calcPos(p);
	ctx.beginPath();
	ctx.fillRect(x*out.width, y*out.height, out.width/64, out.height/64);
	p++;
}, 1);

// async function benchmark() {
// 	for (let p = 0; p < numOfPixels; p += pIncrement) {
// 		let [x, y, w] = calcPos(p);
// 		ctx.beginPath();
// 		ctx.fillRect(x*out.width, y*out.height, pixelSize, pixelSize);
// 	}
// }
// benchmark().then(console.log(performance.now()));
