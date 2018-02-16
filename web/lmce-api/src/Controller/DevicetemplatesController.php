<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Devicetemplates Controller
 *
 * @property \App\Model\Table\DevicetemplatesTable $Devicetemplates
 *
 * @method \App\Model\Entity\Devicetemplate[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class DevicetemplatesController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $this->paginate = [
            'contain' => ['Devicecategory']
        ];
        $devicetemplates = $this->paginate($this->Devicetemplates);

        $this->set(compact('devicetemplates'));
    }

    /**
     * View method
     *
     * @param string|null $id Devicetemplate id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $devicetemplate = $this->Devicetemplates->get($id, [
            'contain' => ['Devicecategory', 'DeviceData', 'Manufacturer']
        ]);

        $this->set('devicetemplate', $devicetemplate);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $devicetemplate = $this->Devicetemplates->newEntity();
        if ($this->request->is('post')) {
            $devicetemplate = $this->Devicetemplates->patchEntity($devicetemplate, $this->request->getData());
            if ($this->Devicetemplates->save($devicetemplate)) {
                $this->Flash->success(__('The devicetemplate has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The devicetemplate could not be saved. Please, try again.'));
        }
        $devicecategory = $this->Devicetemplates->Devicecategory->find('list', ['limit' => 200]);
        $deviceData = $this->Devicetemplates->DeviceData->find('list', ['limit' => 200]);
        $this->set(compact('devicetemplate', 'devicecategory', 'deviceData'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Devicetemplate id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $devicetemplate = $this->Devicetemplates->get($id, [
            'contain' => ['DeviceData']
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $devicetemplate = $this->Devicetemplates->patchEntity($devicetemplate, $this->request->getData());
            if ($this->Devicetemplates->save($devicetemplate)) {
                $this->Flash->success(__('The devicetemplate has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The devicetemplate could not be saved. Please, try again.'));
        }
        $devicecategory = $this->Devicetemplates->Devicecategory->find('list', ['limit' => 200]);
        $deviceData = $this->Devicetemplates->DeviceData->find('list', ['limit' => 200]);
        $this->set(compact('devicetemplate', 'devicecategory', 'deviceData'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Devicetemplate id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $devicetemplate = $this->Devicetemplates->get($id);
        if ($this->Devicetemplates->delete($devicetemplate)) {
            $this->Flash->success(__('The devicetemplate has been deleted.'));
        } else {
            $this->Flash->error(__('The devicetemplate could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
}
