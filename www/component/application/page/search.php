<?php
$q = $_GET["q"];

$categories = array();
require_once("component/data_model/DataModel.inc");
require_once("component/data_model/list/DataList.inc");
$model = DataModel::get();
foreach ($model->getTables() as $table) {
	$displayable = $table->getDisplayableData();
	if (count($displayable) == 0) continue;
	foreach ($displayable as $field=>$localized) {
		$list = new DataList($table->getName());
		$list->add($table->getName().".".$field, false);
		$list->search($table->getName().".".$field, $q);
		$link = $model->getDataCategoryLink($localized[0]);
		if ($link <> null) {
			$i = 0;
			while (($i = strpos($link, "%", $i)) !== FALSE) {
				$j = strpos($link, "%", $i+1);
				if ($j === FALSE) break;
				$f = substr($link, $i+1, $j-$i-1);
				$i = $j+1;
				$list->add($f, false);
			}
		}
		if (PNApplication::has_errors()) {
			// not allowed
			PNApplication::$errors = array();
			continue;
		}
		// remove not unique columns
		for ($i = 0; $i < count($list->columns); $i++)
			if (!$list->columns[$i]->is_unique()) {
				array_splice($list->columns, $i, 1);
				$i--;
			}
		$result = $list->process();
		if ($result["count"] > 0) {
			if (!isset($categories[$localized[0]]))
				$categories[$localized[0]] = array();
			$a = array();
			foreach ($result["list"] as $row) {
				$value = $row[$list->get($table->getName().".".$field)->final_name];
				$r = array($value);
				if ($link <> null) {
					$l = $link;
					foreach ($list->columns as $col)
						if ($col->final_name)
							$l = str_replace("%".$col->get_path()."%", $row[$col->final_name], $l);
					array_push($r, $l);
				}
				array_push($a, $r);
			}
			$categories[$localized[0]][$localized[1]] = $a;
		}
	}
}

$total = 0;
$total_per_cat = array();
foreach ($categories as $cat_name=>$cat_content) {
	$total_per_cat[$cat_name] = 0;
	foreach ($cat_content as $data_name=>$list) {
		// merge same data
		$result = array();
		foreach ($list as $data) {
			$found = false;
			foreach ($result as &$d)
				if ($d[0] == $data[0]) {
					$found = true;
					$d[1]++;
					$d[2] = null; // no link
					break;
				}
			if (!$found)
				array_push($result, array($data[0], 1, @$data[1]<>null && strpos($data[1], "%") === FALSE ? $data[1] : null));
		}
		$categories[$cat_name][$data_name] = $result;
		$nb = count($result);
		$total += $nb;
		$total_per_cat[$cat_name] += $nb;
	}
}

if ($total == 0) {
	locale("No result found for your search");
} else {
	echo $total." ".get_locale("results found").".<br/>";
}

echo "<ul>";
foreach ($categories as $cat_name=>$cat_content) {
	$i = strpos($cat_name, "::");
	$cat_name_locale = get_locale(substr($cat_name,0,$i), substr($cat_name,$i+2));
	echo "<li><span class='category'>".$cat_name_locale."</span> <span class='number'>(".$total_per_cat[$cat_name].")</span><ul>";
	foreach ($cat_content as $data_name=>$list) {
		$i = strpos($data_name, "::");
		$data_name = get_locale(substr($data_name,0,$i), substr($data_name,$i+2));
		echo "<li><span class='type'>".$data_name."</span> <span class='number'>(".count($list).")</span>";
		echo "<ul>";
		foreach ($list as $data) {
			echo "<li>";
			$link = null;
			if ($data[2] <> null) {
				$link = $data[2];
				echo "<a href=\"".$link."\">";
			}
			echo htmlentities($data[0]);
			if ($data[1] > 1) echo " (".$data[1].")";
			if ($link <> null) echo "</a>";
			echo "</li>";
		}
		echo "</ul>";
		echo "</li>";
	}
	echo "</ul>";
	echo "</li>";
}
echo "</ul>";
?>
<style type='text/css'>
.category {
	font-size: 12pt;
	color: #8080FF;
}
.type {
	font-size: 10pt;
	color: #8080FF;
	margin-left: 10px;
	margin-bottom: 0px;
}
ul {
	margin: 2px;
}
li {
	margin: 0px;
	padding: 0px;
}
</style>