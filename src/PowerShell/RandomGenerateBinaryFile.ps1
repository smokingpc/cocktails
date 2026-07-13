
[CmdletBinding(DefaultParameterSetName = 'Bytes')]
param(
	[Parameter(Mandatory = $true, ParameterSetName = 'Bytes')]
	[ValidateRange(1, [long]::MaxValue)]
	[long]$SizeInBytes,

	[Parameter(Mandatory = $true, ParameterSetName = 'KB')]
	[ValidateRange(1, [long]::MaxValue / 1024)]
	[long]$SizeInKB,

	[Parameter(Mandatory = $true, ParameterSetName = 'MB')]
	[ValidateRange(1, [long]::MaxValue / 1024 / 1024)]
	[long]$SizeInMB,

	[Parameter(Mandatory = $true, ParameterSetName = 'Bytes')]
	[Parameter(Mandatory = $true, ParameterSetName = 'KB')]
	[Parameter(Mandatory = $true, ParameterSetName = 'MB')]
	[ValidateNotNullOrEmpty()]
	[string]$OutputFilepath
)

$FileSize = switch ($PSCmdlet.ParameterSetName) {
	'Bytes' { $SizeInBytes }
	'KB'    { $SizeInKB * 1KB }
	'MB'    { $SizeInMB * 1MB }
	default { throw 'A valid size parameter must be provided.' }
}

$bytes = New-Object byte[] $FileSize
$rng = New-Object Security.Cryptography.RNGCryptoServiceProvider
try {
	$rng.GetBytes($bytes)
}
finally {
	$rng.Dispose()
}

[IO.File]::WriteAllBytes($OutputFilepath, $bytes)
