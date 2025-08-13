function Set-WideOutput {
    param(
        [int]$Width = 256,
        [int]$BufferHeight = 8192
    )

    try {
        if ($Host.UI -and $Host.UI.RawUI) {
            $rawUI = $Host.UI.RawUI
            $rawUI.BufferSize = New-Object Management.Automation.Host.Size($Width, $BufferHeight)
            $rawUI.WindowSize = New-Object Management.Automation.Host.Size($Width, $rawUI.WindowSize.Height)
        }
    }
    catch {
        # Non-interactive or redirected output may not support RawUI
    }

    $PSDefaultParameterValues["Format-Table:AutoSize"] = $true
    $PSDefaultParameterValues["Out-String:Width"] = $Width
}

# Apply immediately when PowerShell starts
Set-WideOutput
