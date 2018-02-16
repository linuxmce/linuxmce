<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Devicesmanufacturers Controller
 *
 * @property \App\Model\Table\DevicesmanufacturersTable $Devicesmanufacturers
 *
 * @method \App\Model\Entity\Devicesmanufacturer[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class DevicesmanufacturersController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $this->paginate = [
            'contain' => ['Devices', 'Devicetemplates', 'Manufacturers']
        ];
        $devicesmanufacturers = $this->paginate($this->Devicesmanufacturers);

        $this->set(compact('devicesmanufacturers'));
    }

    /**
     * View method
     *
     * @param string|null $id Devicesmanufacturer id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $devicesmanufacturer = $this->Devicesmanufacturers->get($id, [
            'contain' => ['Devices', 'Devicetemplates', 'Manufacturers']
        ]);

        $this->set('devicesmanufacturer', $devicesmanufacturer);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $devicesmanufacturer = $this->Devicesmanufacturers->newEntity();
        if ($this->request->is('post')) {
            $devicesmanufacturer = $this->Devicesmanufacturers->patchEntity($devicesmanufacturer, $this->request->getData());
            if ($this->Devicesmanufacturers->save($devicesmanufacturer)) {
                $this->Flash->success(__('The devicesmanufacturer has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The devicesmanufacturer could not be saved. Please, try again.'));
        }
        $devices = $this->Devicesmanufacturers->Devices->find('list', ['limit' => 200]);
        $devicetemplates = $this->Devicesmanufacturers->Devicetemplates->find('list', ['limit' => 200]);
        $manufacturers = $this->Devicesmanufacturers->Manufacturers->find('list', ['limit' => 200]);
        $this->set(compact('devicesmanufacturer', 'devices', 'devicetemplates', 'manufacturers'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Devicesmanufacturer id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $devicesmanufacturer = $this->Devicesmanufacturers->get($id, [
            'contain' => []
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $devicesmanufacturer = $this->Devicesmanufacturers->patchEntity($devicesmanufacturer, $this->request->getData());
            if ($this->Devicesmanufacturers->save($devicesmanufacturer)) {
                $this->Flash->success(__('The devicesmanufacturer has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The devicesmanufacturer could not be saved. Please, try again.'));
        }
        $devices = $this->Devicesmanufacturers->Devices->find('list', ['limit' => 200]);
        $devicetemplates = $this->Devicesmanufacturers->Devicetemplates->find('list', ['limit' => 200]);
        $manufacturers = $this->Devicesmanufacturers->Manufacturers->find('list', ['limit' => 200]);
        $this->set(compact('devicesmanufacturer', 'devices', 'devicetemplates', 'manufacturers'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Devicesmanufacturer id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $devicesmanufacturer = $this->Devicesmanufacturers->get($id);
        if ($this->Devicesmanufacturers->delete($devicesmanufacturer)) {
            $this->Flash->success(__('The devicesmanufacturer has been deleted.'));
        } else {
            $this->Flash->error(__('The devicesmanufacturer could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
}
