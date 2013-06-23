function selection_widget(name) {
	var t=this;
	t.container = document.getElementById(name+'_content');
	ajax.call("GET","/dynamic/selection/page/widget_"+name,null,null,
		function(error){
			t.container.innerHTML = "<span style='color:red'>"+error+"</span>";
		},function(xhr){
			t.container.innerHTML = xhr.responseText;
		}
	);
}