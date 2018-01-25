<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Manufacturer Controller
 *
 *
 * @method \App\Model\Entity\Manufacturer[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class ManufacturerController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $manufacturer = $this->paginate($this->Manufacturer);

        $this->set(compact('manufacturer'));
    }

    /**
     * View method
     *
     * @param string|null $id Manufacturer id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $manufacturer = $this->Manufacturer->get($id, [
            'contain' => []
        ]);

        $this->set('manufacturer', $manufacturer);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $manufacturer = $this->Manufacturer->newEntity();
        if ($this->request->is('post')) {
            $manufacturer = $this->Manufacturer->patchEntity($manufacturer, $this->request->getData());
            if ($this->Manufacturer->save($manufacturer)) {
                $this->Flash->success(__('The manufacturer has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The manufacturer could not be saved. Please, try again.'));
        }
        $this->set(compact('manufacturer'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Manufacturer id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $manufacturer = $this->Manufacturer->get($id, [
            'contain' => []
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $manufacturer = $this->Manufacturer->patchEntity($manufacturer, $this->request->getData());
            if ($this->Manufacturer->save($manufacturer)) {
                $this->Flash->success(__('The manufacturer has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The manufacturer could not be saved. Please, try again.'));
        }
        $this->set(compact('manufacturer'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Manufacturer id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $manufacturer = $this->Manufacturer->get($id);
        if ($this->Manufacturer->delete($manufacturer)) {
            $this->Flash->success(__('The manufacturer has been deleted.'));
        } else {
            $this->Flash->error(__('The manufacturer could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
}
