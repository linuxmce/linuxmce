<div class="devices index">
	<h2><?php echo __('Devices'); ?></h2>
	<table cellpadding="0" cellspacing="0">
	<thead>
	<tr>
			<th><?php echo $this->Paginator->sort('PK_Device'); ?></th>
			<th><?php echo $this->Paginator->sort('Disabled'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Room'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Installation'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_DesignObj'); ?></th>
			<th><?php echo $this->Paginator->sort('Description'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_DeviceTemplate'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Device_ControlledVia'); ?></th>
			<th><?php echo $this->Paginator->sort('IPaddress'); ?></th>
			<th><?php echo $this->Paginator->sort('MACaddress'); ?></th>
			<th><?php echo $this->Paginator->sort('IgnoreOnOff'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Device_RouteTo'); ?></th>
			<th><?php echo $this->Paginator->sort('NeedConfigure'); ?></th>
			<th><?php echo $this->Paginator->sort('State'); ?></th>
			<th><?php echo $this->Paginator->sort('Status'); ?></th>
			<th><?php echo $this->Paginator->sort('PingTest'); ?></th>
			<th><?php echo $this->Paginator->sort('EK_LicensedDevice'); ?></th>
			<th><?php echo $this->Paginator->sort('IsNewDevice'); ?></th>
			<th><?php echo $this->Paginator->sort('ManuallyConfigureEA'); ?></th>
			<th><?php echo $this->Paginator->sort('Registered'); ?></th>
			<th><?php echo $this->Paginator->sort('psc_id'); ?></th>
			<th><?php echo $this->Paginator->sort('psc_batch'); ?></th>
			<th><?php echo $this->Paginator->sort('psc_user'); ?></th>
			<th><?php echo $this->Paginator->sort('psc_frozen'); ?></th>
			<th><?php echo $this->Paginator->sort('psc_mod'); ?></th>
			<th><?php echo $this->Paginator->sort('psc_restrict'); ?></th>
			<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	</thead>
	<tbody>
	<?php foreach ($devices as $device): ?>
	<tr>
		<td><?php echo h($device['Device']['PK_Device']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['Disabled']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['FK_Room']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['FK_Installation']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['FK_DesignObj']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['Description']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['FK_DeviceTemplate']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['FK_Device_ControlledVia']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['IPaddress']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['MACaddress']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['IgnoreOnOff']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['FK_Device_RouteTo']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['NeedConfigure']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['State']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['Status']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['PingTest']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['EK_LicensedDevice']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['IsNewDevice']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['ManuallyConfigureEA']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['Registered']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['psc_id']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['psc_batch']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['psc_user']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['psc_frozen']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['psc_mod']); ?>&nbsp;</td>
		<td><?php echo h($device['Device']['psc_restrict']); ?>&nbsp;</td>
		<td class="actions">
			<?php echo $this->Html->link(__('View'), array('action' => 'view', $device['Device']['PK_Device'])); ?>
			<?php echo $this->Html->link(__('Edit'), array('action' => 'edit', $device['Device']['PK_Device'])); ?>
			<?php echo $this->Form->postLink(__('Delete'), array('action' => 'delete', $device['Device']['PK_Device']), array('confirm' => __('Are you sure you want to delete # %s?', $device['Device']['PK_Device']))); ?>
		</td>
	</tr>
<?php endforeach; ?>
	</tbody>
	</table>
	<p>
	<?php
	echo $this->Paginator->counter(array(
		'format' => __('Page {:page} of {:pages}, showing {:current} records out of {:count} total, starting on record {:start}, ending on {:end}')
	));
	?>	</p>
	<div class="paging">
	<?php
		echo $this->Paginator->prev('< ' . __('previous'), array(), null, array('class' => 'prev disabled'));
		echo $this->Paginator->numbers(array('separator' => ''));
		echo $this->Paginator->next(__('next') . ' >', array(), null, array('class' => 'next disabled'));
	?>
	</div>
</div>
<div class="actions">
	<h3><?php echo __('Actions'); ?></h3>
	<ul>
		<li><?php echo $this->Html->link(__('New Device'), array('action' => 'add')); ?></li>
		<li><?php echo $this->Html->link(__('List Device Categories'), array('controller' => 'device_categories', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Category'), array('controller' => 'device_categories', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Command Groups'), array('controller' => 'device_command_groups', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Command Group'), array('controller' => 'device_command_groups', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Templates'), array('controller' => 'device_templates', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Template'), array('controller' => 'device_templates', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Commands'), array('controller' => 'commands', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Command'), array('controller' => 'commands', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Command Groups'), array('controller' => 'command_groups', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Command Group'), array('controller' => 'command_groups', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Data'), array('controller' => 'device_data', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Data'), array('controller' => 'device_data', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Groups'), array('controller' => 'device_groups', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Group'), array('controller' => 'device_groups', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Entertain Areas'), array('controller' => 'entertain_areas', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Entertain Area'), array('controller' => 'entertain_areas', 'action' => 'add')); ?> </li>
	</ul>
</div>
