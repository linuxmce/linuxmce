<div class="deviceTemplates index">
	<h2><?php echo __('Device Templates'); ?></h2>
	<table cellpadding="0" cellspacing="0">
	<thead>
	<tr>
			<th><?php echo $this->Paginator->sort('PK_DeviceTemplate'); ?></th>
			<th><?php echo $this->Paginator->sort('Description'); ?></th>
			<th><?php echo $this->Paginator->sort('Comments'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_DeviceCategory'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Manufacturer'); ?></th>
			<th><?php echo $this->Paginator->sort('Define'); ?></th>
			<th><?php echo $this->Paginator->sort('ImplementsDCE'); ?></th>
			<th><?php echo $this->Paginator->sort('IsEmbedded'); ?></th>
			<th><?php echo $this->Paginator->sort('CommandLine'); ?></th>
			<th><?php echo $this->Paginator->sort('IsPlugAndPlay'); ?></th>
			<th><?php echo $this->Paginator->sort('IsIPBased'); ?></th>
			<th><?php echo $this->Paginator->sort('IsPlugIn'); ?></th>
			<th><?php echo $this->Paginator->sort('IRFrequency'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_StabilityStatus'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Package'); ?></th>
			<th><?php echo $this->Paginator->sort('DestinationDir'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Users_Maintainer'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_OperatingSystem'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Distro'); ?></th>
			<th><?php echo $this->Paginator->sort('InheritsMacFromPC'); ?></th>
			<th><?php echo $this->Paginator->sort('ConfigureScript'); ?></th>
			<th><?php echo $this->Paginator->sort('ManufacturerURL'); ?></th>
			<th><?php echo $this->Paginator->sort('InternalURLSuffix'); ?></th>
			<th><?php echo $this->Paginator->sort('WizardURL'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Licensing'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_InfraredGroup'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_CommMethod'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_PnpLevel'); ?></th>
			<th><?php echo $this->Paginator->sort('HelpURL'); ?></th>
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
	<?php foreach ($deviceTemplates as $deviceTemplate): ?>
	<tr>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['PK_DeviceTemplate']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['Description']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['Comments']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_DeviceCategory']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_Manufacturer']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['Define']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['ImplementsDCE']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['IsEmbedded']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['CommandLine']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['IsPlugAndPlay']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['IsIPBased']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['IsPlugIn']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['IRFrequency']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_StabilityStatus']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_Package']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['DestinationDir']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_Users_Maintainer']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_OperatingSystem']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_Distro']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['InheritsMacFromPC']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['ConfigureScript']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['ManufacturerURL']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['InternalURLSuffix']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['WizardURL']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_Licensing']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_InfraredGroup']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_CommMethod']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['FK_PnpLevel']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['HelpURL']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['psc_id']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['psc_batch']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['psc_user']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['psc_frozen']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['psc_mod']); ?>&nbsp;</td>
		<td><?php echo h($deviceTemplate['DeviceTemplate']['psc_restrict']); ?>&nbsp;</td>
		<td class="actions">
			<?php echo $this->Html->link(__('View'), array('action' => 'view', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate'])); ?>
			<?php echo $this->Html->link(__('Edit'), array('action' => 'edit', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate'])); ?>
			<?php echo $this->Form->postLink(__('Delete'), array('action' => 'delete', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate']), array('confirm' => __('Are you sure you want to delete # %s?', $deviceTemplate['DeviceTemplate']['PK_DeviceTemplate']))); ?>
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
		<li><?php echo $this->Html->link(__('New Device Template'), array('action' => 'add')); ?></li>
		<li><?php echo $this->Html->link(__('List Devices'), array('controller' => 'devices', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device'), array('controller' => 'devices', 'action' => 'add')); ?> </li>
	</ul>
</div>
