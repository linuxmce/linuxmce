<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Devicecategory Controller
 *
 *
 * @method \App\Model\Entity\Devicecategory[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class DevicecategoryController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $devicecategory = $this->paginate($this->Devicecategory);

        $this->set(compact('devicecategory'));
    }

    /**
     * View method
     *
     * @param string|null $id Devicecategory id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $devicecategory = $this->Devicecategory->get($id, [
            'contain' => []
        ]);

        $this->set('devicecategory', $devicecategory);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $devicecategory = $this->Devicecategory->newEntity();
        if ($this->request->is('post')) {
            $devicecategory = $this->Devicecategory->patchEntity($devicecategory, $this->request->getData());
            if ($this->Devicecategory->save($devicecategory)) {
                $this->Flash->success(__('The devicecategory has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The devicecategory could not be saved. Please, try again.'));
        }
        $this->set(compact('devicecategory'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Devicecategory id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $devicecategory = $this->Devicecategory->get($id, [
            'contain' => []
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $devicecategory = $this->Devicecategory->patchEntity($devicecategory, $this->request->getData());
            if ($this->Devicecategory->save($devicecategory)) {
                $this->Flash->success(__('The devicecategory has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The devicecategory could not be saved. Please, try again.'));
        }
        $this->set(compact('devicecategory'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Devicecategory id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $devicecategory = $this->Devicecategory->get($id);
        if ($this->Devicecategory->delete($devicecategory)) {
            $this->Flash->success(__('The devicecategory has been deleted.'));
        } else {
            $this->Flash->error(__('The devicecategory could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
}
