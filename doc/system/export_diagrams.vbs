Set objShell = CreateObject("Wscript.Shell")
strPath = Wscript.ScriptFullName
Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objFile = objFSO.GetFile(strPath)
strFolder = objFSO.GetParentFolderName(objFile)

Set app = CreateObject("StarUML.StarUMLApplication")
Set prjmgr = app.ProjectManager
prjmgr.OpenProject(strFolder & "\StudentsDatabase.uml")
Set prj = app.GetProject()

Sub VisitOwnedElement(owner,path)
	For i = 0 To owner.GetOwnedDiagramCount()-1
		Set diagram = owner.GetOwnedDiagramAt(i)
		diagram.DiagramView.ExportDiagramAsMetafile strFolder & "\diagrams\" & path & diagram.Name & ".wmf"
		REM diagram.DiagramView.ExportDiagramAsJPEG strFolder & "\diagrams\" & path & diagram.Name & ".wmf"
		REM diagram.DiagramView.ExportDiagramAsBitmap strFolder & "\diagrams\" & path & diagram.Name & ".wmf"
	Next

	For i = 0 To owner.GetOwnedElementCount()-1
		Set elem = owner.GetOwnedElementAt(i)
		If elem.IsKindOf("UMLNamespace") Then
			VisitOwnedElement elem, path & elem.Name & "_"
		End If
	Next
End Sub

VisitOwnedElement prj,""

