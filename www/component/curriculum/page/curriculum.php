<?php
$id = $_GET["id"];
$edit = isset($_GET["edit"]);

$can_edit = PNApplication::$instance->user_management->has_right("edit_curricula");
$edit &= $can_edit;

require_once("common/DataBaseLock.inc");
$locked_by = null;
if ($edit)
	$lock_id = DataBaseLock::lock("Curriculum", array("id",$id), $locked_by);

require_once("common/SQLQuery.inc");
$name = SQLQuery::create()->select("Curriculum")->field('name')->where("id",$id)->execute_single_value();

require_once("component/application/SubPageHeader.inc");
$header = new SubPageHeader($this,'/static/curriculum/curriculum_32.png', get_locale("Curriculum").": <span style='font-family:Courrier New;font-weight:bold;font-style:italic'>".$name."</span>");
if ($locked_by <> null) {
	$header->add_header("<img src='/static/common/images/lock.png'/> ".get_locale("common","This page is already locked by")." ".$locked_by);
	$header->generate();
	return;
}
if (!$edit && $can_edit)
	$header->add_header("<div class='button' onclick=\"location.href = location.href+'&edit';\"><img src='/static/common/images/edit.png'/> ".get_locale("common", "Edit")."</div>");

$list = SQLQuery::create()->select("CurriculumBranch")->where("curriculum",$id)->execute();
$branches = array();
function add_branch(&$branches, $branch) {
	$found = false;
	foreach ($branches as &$b) {
		if ($b["id"] == $branch["previous"]) {
			array_push($b["sub_branches"], $branch);
			$found = true;
			break;
		}
	}
	if (!$found) {
		foreach ($branches as &$b) {
			if (count($b["sub_branches"]) > 0)
				if (add_branch($b["sub_branches"], $branch)) {
					$found = true;
					break;
				}
		}
	}
	return $found;
}
while (count($list) > 0) {
	$rem = array();
	foreach ($list as $branch) {
		$branch["sub_branches"] = array();
		if ($branch["previous"] == null)
			array_push($branches, $branch);
		else
			if (!add_branch($branches, $branch))
				array_push($rem, $branch);
	}
	$list = $rem;
};

if (count($branches) == 0 && $edit)
	$header->add_header("<div class='button' onclick=\"curriculum_add_branch(0);\"><img src='/static/curriculum/branch_16.png'/> ".get_locale("Create root branch")."</div>");

$header->generate();

if ($edit) {
?>
<script type='text/javascript'>
function curriculum_add_branch(previous_id) {
	pn.input_dialog('/static/curriculum/branch_16.png',"<?php locale("Create branch")?>","<?php locale("common","Name")?>","",100,function(name){
		if (name.length == 0)
			return "<?php locale("common","Cannot be empty")?>";
		return null;
	},function(name){
		// TODO create
	});
}
</script>
<?php }?>