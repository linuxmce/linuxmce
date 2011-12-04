#!/usr/bin/perl

package DCE_Whisperer;

use warnings;
use strict;

use POSIX;
use Switch;

use IO::Pty;
use IO::Socket::INET;

use URI::Escape;
use Symbol;

sub new()
{
	my $self = {};
	my $type = $_[0];
	bless $self, $type;

	$self->ParseArgs();
	$self->{"Configuration"} = main::Configure();

	$self->{"DCEfh"} = gensym;
	$self->{"DCEpid"} = undef;

	$self->{"DelayDCE"} = 0;
	$self->{"DCEqueue"} = [];
	$self->{"Quit"} = 0;

	return $self;
}

sub Run()
{
	my $self = shift;

	$self->ConnectDCE();
	$self->ConnectDevice();

	main::OnInit();

	until ($self->{"Quit"})
	{
		my $Rbits = '';
		vec($Rbits, fileno($self->{"DCEfh"}), 1) = 1;
		vec($Rbits, fileno($self->{"DeviceFH"}), 1) = 1 if defined($self->{"DeviceFH"});

		my $nfound = select($Rbits, undef, undef, undef);

		if (vec $Rbits, fileno($self->{"DCEfh"}), 1)
		{
			$self->ReadFromDCE() or $self->{"Quit"} = 1;
		}
		elsif (defined($self->{"DeviceFH"}) && vec $Rbits, fileno($self->{"DeviceFH"}), 1)
		{
			if (!$self->ReadFromDevice())
			{
				if ($self->{"Configuration"}->{"DeviceConnection_OnDisconnect"} eq "reconnect")
				{
					$self->ConnectDevice();
				}
				else
				{
					# TODO: Disable device
					$self->{"Quit"} = 1;
				}
			}
		}
	};

	undef $self->{"DCEfh"};

	waitpid $self->{"DCEpid"}, 0;
	my $ExitCode = WEXITSTATUS($?);
	print "Whisperer exited with status $ExitCode\n";

	main::OnExit();

	exit $ExitCode;
}

## Send a DCE message
sub SendToDCE($$$$\%)
{
	my ($self, $From, $To, $Type, $MsgID, $Parameters) = @_;

	my @Parameters = ($From, $To, $Type, $MsgID);
	foreach my $ParmID (keys %$Parameters)
	{
		push(@Parameters, $ParmID . " " . uri_escape($Parameters->{$ParmID}));
	}
	my $MessageString = join(" ", @Parameters);

	$self->SendToDCE_String($MessageString);
}

## Send data to device
sub SendToDevice($)
{
	my $self = shift;
	my $Data = \$_[0];

	return if (!defined($self->{"DeviceFH"}));

	syswrite $self->{"DeviceFH"}, $$Data;
	syswrite $self->{"DeviceFH"}, $self->{"Configuration"}->{"DeviceProtocol_Separator"}
		if $self->{"Configuration"}->{"DeviceProtocol_AutoAppendSeparator"} eq "yes";

	if (defined($self->{"Configuration"}->{"DeviceProtocol_Delay"}) && $self->{"Configuration"}->{"DeviceProtocol_Delay"} > 0)
	{
		select (undef, undef, undef, $self->{"Configuration"}->{"DeviceProtocol_Delay"});
	}
}

########################################
# Private ##############################
########################################

sub SendToDCE_String($)
{
	my ($self, $MessageString) = @_;

	if ($self->{"DelayDCE"})
	{
		print "Queuing DCE command: $MessageString\n";
		push(@{$self->{"DCEqueue"}}, $MessageString);
	}
	else
	{
		print "Sending DCE command: $MessageString\n";
		print {$self->{"DCEfh"}} "$MessageString\n";
	}
}

sub SendDelayedDCE()
{
	my ($self) = shift;

	if ($self->{"DelayDCE"})
	{
		print "SendDelayedDCE: called while delay is still active\n";
		return;
	}
	print "Sending out delayed DCE messages\n";

	foreach my $MessageString (@{$self->{"DCEqueue"}})
	{
		$self->SendToDCE_String($MessageString);
	}
	$self->{"DCEqueue"} = [];
}

sub ParseArgs()
{
	my $self = shift;

	$self->{"Router"} = "dcerouter";
	for (my $i = 0; $i < scalar @ARGV; $i += 2)
	{
		$self->{"PK_Device"} = $ARGV[$i + 1] if $ARGV[$i] eq "-d";
		$self->{"Router"} = $ARGV[$i + 1] if $ARGV[$i] eq "-r";
		$self->{"LogFile"} = $ARGV[$i + 1] if $ARGV[$i] eq "-l";
	}

	if (!defined($self->{"PK_Device"}) || $self->{"PK_Device"} eq "")
	{
		die "ERROR: Device Number not specified";
	}
}

sub ConnectDCE()
{
	my $self = shift;
	$self->{"DCEpid"} = open_pty($self->{"DCEfh"}, "/usr/pluto/bin/DCE-Whisperer", "-d", $self->{"PK_Device"}, "-r", $self->{"Router"});
	die "Failed to start DCE-Whisperer" unless defined $self->{"DCEpid"};
}

sub ConnectDevice()
{
	my $self = shift;
	
	switch ($self->{"Configuration"}->{"DeviceConnection_Type"})
	{
		case "none"
		{
			print "ConnectDevice: No device to connect to\n";
			return 0;
		}

		case "serial"
		{
			my $oldF = $^F;
			$^F = 1024;
			if (! open($self->{"DeviceFH"}, "+>", $self->{"Configuration"}->{"DeviceConnection_SerialPort"}))
			{
				print "ConnectDevice: Failed to open serial port: $!\n";
				return 1;
			}
			my @args = (
				"/usr/pluto/bin/TestSerialPort",
				"-p", "/dev/fd/" . fileno($self->{"DeviceFH"}),
				"-b", $self->{"Configuration"}->{"DeviceConnection_BaudRate"},
				"-P", $self->{"Configuration"}->{"DeviceConnection_Parity"},
			);
			system {$args[0]} @args;

			@args = (
				"/bin/stty",
				"--file=/dev/fd/" . fileno($self->{"DeviceFH"}),
				"-icanon", "raw", "min", "1",
			);
			system {$args[0]} @args;

			$^F = $oldF;
		}

		case "inet"
		{
			if (! ($self->{"DeviceFH"} = IO::Socket::INET->new(
				PeerAddr => $self->{"Configuration"}->{"DeviceConnection_Endpoint"},
				Proto => $self->{"Configuration"}->{"DeviceConnection_Protocol"},
			)))
			{
				print "ConnectDevice: Failed to connect to device: $!\n";
				return 1;
			}
		}

		case "custom"
		{
			if (! open_pty($self->{"DeviceFH"}, @{$self->{"Configuration"}->{"DeviceConnection_Command"}}))
			{
				print "ConnectDevice: Failed to connect to device: $!\n";
				return 1;
			}
		}
	}

	main::OnDeviceConnect();

	return 0;
}

sub ReadFromDCE()
{
	my $self = shift;

	my $DCEmsg = readline($self->{"DCEfh"});
	if ($DCEmsg)
	{
		chomp($DCEmsg);
		print("Got from DCERouter: $DCEmsg\n");

		my @DCEmsg = split / /, $DCEmsg;
		my ($Reply, $From, $To, $Type, $MsgID, @Parameters) = @DCEmsg;

		my %Parameters;
		for (my $i = 0; $i < scalar @Parameters; $i += 2)
		{
			$Parameters{$Parameters[$i]} = uri_unescape($Parameters[$i + 1]);
		}

		undef $DCEmsg;
		undef @DCEmsg;
		undef @Parameters;

		if ($Type == 1)
		{
			my $CommandResult = "UNHANDLED";
			my $OutputVariables = {};

			my $CommandFunction = \&{"main::Cmd_$MsgID"};
			if ($To != $self->{"PK_Device"})
			{
				main::ReceivedCommandForChild($From, $To, $Type, $MsgID, \%Parameters);
			}
			elsif (defined(&$CommandFunction))
			{
				$self->{"DelayDCE"} = 1;
				($CommandResult, $OutputVariables) = $CommandFunction->($From, $To, \%Parameters);
				$self->{"DelayDCE"} = 0;
			}
			else
			{
				main::ReceivedUnknownCommand($From, $To, $Type, $MsgID, \%Parameters);
			}

			if ($Reply eq "-r")
			{
				print {$self->{"DCEfh"}} "reply txt $CommandResult\n";
			}
			elsif ($Reply eq "-o")
			{
				my @Variables;
				for my $key (keys %$OutputVariables)
				{
					push(@Variables, $key . " " . uri_escape($OutputVariables->{$key}));
				}
				my $OutputVariables = join(" ", @Variables);
				print {$self->{"DCEfh"}} "reply msg $To $From 4 0 0 $CommandResult $OutputVariables\n";
			}
		}
		$self->SendDelayedDCE();
		return 1;
	}
	else
	{
		return undef;
	}
}

sub ReadFromDevice()
{
	my $self = shift;

	print "Processing incoming data from the device\n";

	my $buffer = undef;
	if ($self->{"Configuration"}->{"DeviceProtocol_Type"} eq "line")
	{
		$self->{"DeviceFH"}->input_record_separator($self->{"Configuration"}->{"DeviceProtocol_Separator"});
		$buffer = readline $self->{"DeviceFH"};
	}
	else # stream
	{
		if (! sysread $self->{"DeviceFH"}, $buffer, 1024)
		{
			undef $buffer;
		}
	}

	if (!defined($buffer))
	{
		print "Connection to the device was lost\n";
		undef $self->{"DeviceFH"};
		return undef;
	}
	else
	{
		main::ProcessDeviceStream($buffer);
		return 1;
	}
}

## spawns an application and connects its stdin and stdout to a pty
sub open_pty
{
	my $FH = \$_[0];
	shift;
	my (@args) = @_;

	my $oldF = $^F;
	$^F = 1024;

	my ($pty, $slave);

	$pty = new IO::Pty;
	$slave = $pty->slave;

	$pty->slave->set_raw;
	$pty->set_raw;

	my $pid = fork();

	if ($pid == 0)
	{
		## Child process
		$pty->make_slave_controlling_terminal;

		open STDIN, "<&" . $slave->fileno;
		open STDOUT, ">&" . $slave->fileno;

		$slave->close;
		$pty->close;

		{ exec {$args[0]} @args; } # curly braces required to suppress a warning, because "_exit" isn't in perl's list for stuff that can follow "exec"
		_exit 127; # reached only if exec fails
	}

	## Parent process
	$^F = $oldF;

	$pty->close_slave;

	if (defined($pid))
	{
		$$FH = $pty;

		return $pid;
	}
	else
	{
		$pty->close;
		return undef;
	}
}

return 1;
