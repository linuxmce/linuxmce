<div class="devices view">
<h2><?php echo __('Device'); ?></h2>
	<dl>
		<dt><?php echo __('PK Device'); ?></dt>
		<dd>
			<?php echo h($device['Device']['PK_Device']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Disabled'); ?></dt>
		<dd>
			<?php echo h($device['Device']['Disabled']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Room'); ?></dt>
		<dd>
			<?php echo h($device['Device']['FK_Room']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Installation'); ?></dt>
		<dd>
			<?php echo h($device['Device']['FK_Installation']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK DesignObj'); ?></dt>
		<dd>
			<?php echo h($device['Device']['FK_DesignObj']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Description'); ?></dt>
		<dd>
			<?php echo h($device['Device']['Description']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Device Template'); ?></dt>
		<dd>
			<?php echo $this->Html->link($device['DeviceTemplate']['PK_DeviceTemplate'], array('controller' => 'DeviceTemplate', 'action' => 'view', $device['DeviceTemplate']['PK_DeviceTemplate'])); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Device ControlledVia'); ?></dt>
		<dd>
			<?php echo h($device['Device']['FK_Device_ControlledVia']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IPaddress'); ?></dt>
		<dd>
			<?php echo h($device['Device']['IPaddress']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('MACaddress'); ?></dt>
		<dd>
			<?php echo h($device['Device']['MACaddress']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IgnoreOnOff'); ?></dt>
		<dd>
			<?php echo h($device['Device']['IgnoreOnOff']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('FK Device RouteTo'); ?></dt>
		<dd>
			<?php echo h($device['Device']['FK_Device_RouteTo']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('NeedConfigure'); ?></dt>
		<dd>
			<?php echo h($device['Device']['NeedConfigure']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('State'); ?></dt>
		<dd>
			<?php echo h($device['Device']['State']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Status'); ?></dt>
		<dd>
			<?php echo h($device['Device']['Status']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('PingTest'); ?></dt>
		<dd>
			<?php echo h($device['Device']['PingTest']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('EK LicensedDevice'); ?></dt>
		<dd>
			<?php echo h($device['Device']['EK_LicensedDevice']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('IsNewDevice'); ?></dt>
		<dd>
			<?php echo h($device['Device']['IsNewDevice']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('ManuallyConfigureEA'); ?></dt>
		<dd>
			<?php echo h($device['Device']['ManuallyConfigureEA']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Registered'); ?></dt>
		<dd>
			<?php echo h($device['Device']['Registered']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Id'); ?></dt>
		<dd>
			<?php echo h($device['Device']['psc_id']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Batch'); ?></dt>
		<dd>
			<?php echo h($device['Device']['psc_batch']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc User'); ?></dt>
		<dd>
			<?php echo h($device['Device']['psc_user']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Frozen'); ?></dt>
		<dd>
			<?php echo h($device['Device']['psc_frozen']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Mod'); ?></dt>
		<dd>
			<?php echo h($device['Device']['psc_mod']); ?>
			&nbsp;
		</dd>
		<dt><?php echo __('Psc Restrict'); ?></dt>
		<dd>
			<?php echo h($device['Device']['psc_restrict']); ?>
			&nbsp;
		</dd>
	</dl>
</div>
<div class="actions">
	<h3><?php echo __('Actions'); ?></h3>
	<ul>
		<li><?php echo $this->Html->link(__('Edit Device'), array('action' => 'edit', $device['Device']['PK_Device'])); ?> </li>
		<li><?php echo $this->Form->postLink(__('Delete Device'), array('action' => 'delete', $device['Device']['PK_Device']), array('confirm' => __('Are you sure you want to delete # %s?', $device['Device']['PK_Device']))); ?> </li>
		<li><?php echo $this->Html->link(__('List Devices'), array('action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device'), array('action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Templates'), array('controller' => 'device_templates', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Template'), array('controller' => 'device_templates', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Device Data'), array('controller' => 'device_data', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device Data'), array('controller' => 'device_data', 'action' => 'add')); ?> </li>
	</ul>
</div>
<div class="related">
	<h3><?php echo __('Related Device Data'); ?></h3>
	<?php if (!empty($device['DeviceData'])): ?>
	<table cellpadding = "0" cellspacing = "0">
	<tr>
		<th><?php echo __('PK DeviceData'); ?></th>
		<th><?php echo __('Description'); ?></th>
		<th><?php echo __('Define'); ?></th>
		<th><?php echo __('FK ParameterType'); ?></th>
		<th><?php echo __('Psc Id'); ?></th>
		<th><?php echo __('Psc Batch'); ?></th>
		<th><?php echo __('Psc User'); ?></th>
		<th><?php echo __('Psc Frozen'); ?></th>
		<th><?php echo __('Psc Mod'); ?></th>
		<th><?php echo __('Psc Restrict'); ?></th>
		<th class="actions"><?php echo __('Actions'); ?></th>
	</tr>
	<?php foreach ($device['DeviceData'] as $deviceData): ?>
		<tr>
			<td><?php echo $deviceData['PK_DeviceData']; ?></td>
			<td><?php echo $deviceData['Description']; ?></td>
			<td><?php echo $deviceData['Define']; ?></td>
			<td><?php echo $deviceData['FK_ParameterType']; ?></td>
			<td><?php echo $deviceData['psc_id']; ?></td>
			<td><?php echo $deviceData['psc_batch']; ?></td>
			<td><?php echo $deviceData['psc_user']; ?></td>
			<td><?php echo $deviceData['psc_frozen']; ?></td>
			<td><?php echo $deviceData['psc_mod']; ?></td>
			<td><?php echo $deviceData['psc_restrict']; ?></td>
			<td class="actions">
				<?php echo $this->Html->link(__('View'), array('controller' => 'device_data', 'action' => 'view', $deviceData['PK_DeviceData'])); ?>
				<?php echo $this->Html->link(__('Edit'), array('controller' => 'device_data', 'action' => 'edit', $deviceData['PK_DeviceData'])); ?>
				<?php echo $this->Form->postLink(__('Delete'), array('controller' => 'device_data', 'action' => 'delete', $deviceData['PK_DeviceData']), array('confirm' => __('Are you sure you want to delete # %s?', $deviceData['PK_DeviceData']))); ?>
			</td>
		</tr>
	<?php endforeach; ?>
	</table>
<?php endif; ?>

	<div class="actions">
		<ul>
			<li><?php echo $this->Html->link(__('New Device Data'), array('controller' => 'device_data', 'action' => 'add')); ?> </li>
		</ul>
	</div>
</div>
