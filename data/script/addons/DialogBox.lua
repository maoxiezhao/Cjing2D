-- 对话框

function DialogBox.OnLoad()
	print("Dialog Load")
end

function DialogBox.OnUnLoad()
	print("Dialog UnLoad")
end


function DialogBox.OpenDialog(dialogIDs)
	print("Open Dialog:")
	for _,id in ipairs(dialogIDs) do
		print(id)
	end
	
end

function DialogBox.CloseDialog()
	
end

-- Addon Exports
AddonExports.OpenDialog = DialogBox.OpenDialog
AddonExports.CloseDialog = DialogBox.CloseDialog