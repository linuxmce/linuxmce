<div class="users index">
	<h2><?php echo __('Users'); ?></h2>
	<table cellpadding="0" cellspacing="0">
	<thead>
	<tr>
			<th><?php echo $this->Paginator->sort('PK_Users'); ?></th>
			<th><?php echo $this->Paginator->sort('UserName'); ?></th>
			<th><?php echo $this->Paginator->sort('Password'); ?></th>
			<th><?php echo $this->Paginator->sort('PINCode'); ?></th>
			<th><?php echo $this->Paginator->sort('HasMailbox'); ?></th>
			<th><?php echo $this->Paginator->sort('AccessGeneralMailbox'); ?></th>
			<th><?php echo $this->Paginator->sort('Extension'); ?></th>
			<th><?php echo $this->Paginator->sort('FirstName'); ?></th>
			<th><?php echo $this->Paginator->sort('LastName'); ?></th>
			<th><?php echo $this->Paginator->sort('Nickname'); ?></th>
			<th><?php echo $this->Paginator->sort('ExtensionRingTimeout'); ?></th>
			<th><?php echo $this->Paginator->sort('ForwardEmail'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Language'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_Installation_Main'); ?></th>
			<th><?php echo $this->Paginator->sort('HideFromOrbiter'); ?></th>
			<th><?php echo $this->Paginator->sort('Staff'); ?></th>
			<th><?php echo $this->Paginator->sort('Password_Unix'); ?></th>
			<th><?php echo $this->Paginator->sort('Password_Samba'); ?></th>
			<th><?php echo $this->Paginator->sort('FK_UserMode'); ?></th>
			<th><?php echo $this->Paginator->sort('EK_Dealer'); ?></th>
			<th><?php echo $this->Paginator->sort('EK_Manufacturer'); ?></th>
			<th><?php echo $this->Paginator->sort('RequirePinToSelect'); ?></th>
			<th><?php echo $this->Paginator->sort('Use_VPN'); ?></th>
			<th><?php echo $this->Paginator->sort('VPN_Ip'); ?></th>
			<th><?php echo $this->Paginator->sort('Connect_to'); ?></th>
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
	<?php foreach ($users as $user): ?>
	<tr>
		<td><?php echo h($user['User']['PK_Users']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['UserName']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Password']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['PINCode']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['HasMailbox']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['AccessGeneralMailbox']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Extension']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['FirstName']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['LastName']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Nickname']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['ExtensionRingTimeout']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['ForwardEmail']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['FK_Language']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['FK_Installation_Main']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['HideFromOrbiter']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Staff']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Password_Unix']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Password_Samba']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['FK_UserMode']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['EK_Dealer']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['EK_Manufacturer']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['RequirePinToSelect']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Use_VPN']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['VPN_Ip']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['Connect_to']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['psc_id']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['psc_batch']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['psc_user']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['psc_frozen']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['psc_mod']); ?>&nbsp;</td>
		<td><?php echo h($user['User']['psc_restrict']); ?>&nbsp;</td>
		<td class="actions">
			<?php echo $this->Html->link(__('View'), array('action' => 'view', $user['User']['PK_Users'])); ?>
			<?php echo $this->Html->link(__('Edit'), array('action' => 'edit', $user['User']['PK_Users'])); ?>
			<?php echo $this->Form->postLink(__('Delete'), array('action' => 'delete', $user['User']['PK_Users']), array('confirm' => __('Are you sure you want to delete # %s?', $user['User']['PK_Users']))); ?>
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
		<li><?php echo $this->Html->link(__('New User'), array('action' => 'add')); ?></li>
		<li><?php echo $this->Html->link(__('List Devices'), array('controller' => 'devices', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Device'), array('controller' => 'devices', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Installations'), array('controller' => 'installations', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Installation'), array('controller' => 'installations', 'action' => 'add')); ?> </li>
		<li><?php echo $this->Html->link(__('List Rooms'), array('controller' => 'rooms', 'action' => 'index')); ?> </li>
		<li><?php echo $this->Html->link(__('New Room'), array('controller' => 'rooms', 'action' => 'add')); ?> </li>
	</ul>
</div>
