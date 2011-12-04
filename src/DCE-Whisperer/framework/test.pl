#!/usr/bin/perl

use DCE_Whisperer;
use warnings;
use strict;

my $DCE;
my %Configuration;

sub Configure()
{
	%Configuration = (
		## Device connection generics
		"DeviceConnection_Type" => "inet", # choices: none, serial, inet, custom
		## Connection parameters
		# type=serial:
		"DeviceConnection_BaudRate" => 9600,
		"DeviceConnection_Parity" => "N81", # choices: N81, E81, O81
		"DeviceConnection_SerialPort" => "/dev/ttyS1",
		# type=inet:
		"DeviceConnection_Protocol" => "tcp", # choices: tcp, udp
		"DeviceConnection_Endpoint" => "127.0.0.1:5000", # Address:Port
		# type=custom:
		"DeviceConnection_Command" => [],

		## What to do when the device is lost
		"DeviceConnection_OnDisconnect" => "reconnect", # choices: disable, reconnect

		## Device protocol generics
		"DeviceProtocol_Type" => "line", # choices: line, stream
		# type=line:
		"DeviceProtocol_Separator" => "\n",

		"DeviceProtocol_AutoAppendSeparator" => "yes", # choices: yes, no
		"DeviceProtocol_Delay" => .2, #delay between commands (seconds)
	);

	return \%Configuration;
}

sub ProcessDeviceStream($)
{
	my $Data = \$_[0];

	if ($Configuration{"DeviceProtocol_Type"} eq "line")
	{
		my $oldSepatator = $/;
		$/ = $Configuration{"DeviceProtocol_Separator"};
		while (chomp($$Data)) {};
		$/ = $oldSepatator;
	}
	print "Device said: $$Data\n";
}

sub OnInit()
{
	print "*** Init ***\n";
}

sub OnExit()
{
	print "*** Exit ***\n";
}

sub OnDeviceConnect()
{
	print "*** Device connected ***\n";
	$DCE->SendToDevice("alpha");
}

sub ReceivedCommandForChild($$$$\%)
{
	my ($From, $To, $Type, $MsgID, $Parameters) = @_;

	print "Received command $MsgID for child $To\n";
	foreach my $ParmID (keys %$Parameters)
	{
		print "Parm $ParmID: " . $Parameters->{$ParmID} . "\n";
	}

	return ("UNHANDLED", {});
}

sub ReceivedUnknownCommand($$$$\%)
{
	my ($From, $To, $Type, $MsgID, $Parameters) = @_;

	print "Received unknown command $MsgID\n";
	foreach my $ParmID (keys %$Parameters)
	{
		print "Parm $ParmID: " . $Parameters->{$ParmID} . "\n";
	}

	return ("UNHANDLED", {});
}

sub Cmd_1000($$\%)
{
	my ($From, $To, $Parameters) = @_;
	print "Command 1000\n";
	print "Parameters:\n";
	foreach my $ParmID (keys %$Parameters)
	{
		print "Parm $ParmID: " . $Parameters->{$ParmID} . "\n";
	}

	$DCE->SendToDCE($DCE->{"PK_Device"}, $DCE->{"PK_Device"}, 1, 1001, {});

	return ("OK", {});
}

$DCE = new DCE_Whisperer();
$DCE->Run();
