<form name="upload" action="/dynamic/storage/service/upload" method="POST" enctype="multipart/form-data">
	<input type="file" id="fileselect" name="fileselect[]" multiple="multiple" />
	<div id="filedrag">or drop files here</div>
	<button id="submitbutton" type="submit">Upload Files</button>
	<table id="progress_table">
	</table>
</form>

<script type="text/javascript">
// check if File API is supported (HTML 5)
if (window.File && window.FileList && window.FileReader) {
	var fileselect = document.getElementById("fileselect");
	var filedrag = document.getElementById("filedrag"); 
	var submitbutton = document.getElementById("submitbutton");
	fileselect.addEventListener("change", FileSelectHandler, false);
	// is XHR2 available?
	var xhr = new XMLHttpRequest();
	if (xhr.upload) {
		// file drop
		filedrag.addEventListener("dragover", FileDragHover, false);
		filedrag.addEventListener("dragleave", FileDragHover, false);
		filedrag.addEventListener("drop", FileSelectHandler, false);
		filedrag.style.display = "block";
		// remove submit button
		submitbutton.style.display = "none";
	}
}

function FileDragHover(e) {
	e.stopPropagation();
	e.preventDefault();
	e.target.className = (e.type == "dragover" ? "hover" : "");
}

function FileSelectHandler(e) {
	// cancel event and hover styling
	FileDragHover(e);
	// fetch FileList object
	var files = e.target.files || e.dataTransfer.files;
	// process all File objects
	for (var i = 0, f; f = files[i]; i++) {
		if (CheckFile(f))
			UploadFile(f);
	}
}

function CheckFile(f) {
	// TODO check according to the restriction we may want
	return true;
}

function UploadFile(file) {
	var xhr = new XMLHttpRequest();
	if (xhr.upload) {
		// start upload
		xhr.open("POST", "/dynamic/storage/service/upload", true);
		xhr.setRequestHeader("X_FILENAME", file.name);
		var table = document.getElementById("progress_table");
		var tr = document.createElement("TR"); table.appendChild(tr);
		var td = document.createElement("TD");
		td.innerHTML = file.name;
		tr.appendChild(td);
		td = document.createElement("TD"); tr.appendChild(td);
		var progress = document.createElement("DIV");
		progress.style.width="200px";
		progress.style.height="15px";
		progress.style.border="1px solid black";
		progress.style.position="relative";
		var progress_bar = document.createElement("DIV");
		progress_bar.style.position="absolute";
		progress_bar.style.top="0px";
		progress_bar.style.left="0px";
		progress_bar.style.width="0px";
		progress_bar.style.height="15px";
		progress_bar.style.backgroundColor="#A0A0FF";
		progress.appendChild(progress_bar);
		td.appendChild(progress);
		xhr.upload.addEventListener("progress", function(e) {
			progress_bar.style.width = Math.round(e.loaded*200/e.total)+"px";
		}, false);
		xhr.onreadystatechange = function(e) {
			if (xhr.readyState == 4) {
				td.innerHTML = (xhr.status == 200 ? "OK" : "Failed")
			}
		};
		xhr.send(file);
	}
}
</script>
<style type="text/css">
#filedrag
{
	display: none;
	font-weight: bold;
	text-align: center;
	padding: 1em 0;
	margin: 1em 0;
	color: #555;
	border: 2px dashed #555;
	border-radius: 7px;
	cursor: default;
}
#filedrag.hover
{
	color: #f00;
	border-color: #f00;
	border-style: solid;
	box-shadow: inset 0 3px 4px #888;
}
</style>