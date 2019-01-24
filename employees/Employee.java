/*
Copyright (c) 2018-2019, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import lib.util.persistent.*;
import lib.util.persistent.types.*;

public final class Employee extends PersistentObject {
	private static final LongField ID = new LongField();
	private static final StringField NAME = new StringField();
	private static final StringField DEPARTMENT = new StringField();
	private static final ObjectType<Employee> TYPE = ObjectType.withFields(Employee.class, ID, NAME, DEPARTMENT);

	public Employee (long id, PersistentString name, PersistentString department) {
		super(TYPE);
		setLongField(ID, id);
		setName(name);
		setDepartment(department);
	}

	private Employee (ObjectPointer<Employee> p) { 
		super(p); 
	}

	public long getId() {
		return getLongField(ID);
	}
	
	public PersistentString getName() {
		return getObjectField(NAME);
	}
	
	public PersistentString getDepartment() {
		return getObjectField(DEPARTMENT);
	}

	public void setName(PersistentString name) {
		setObjectField(NAME, name);
	}

	public void setDepartment(PersistentString department) {
		setObjectField(DEPARTMENT, department);
	}

	public int hashCode() {
		return Long.hashCode(getId());
	}

	public boolean equals(Object obj) {
		if (!(obj instanceof Employee)) return false;
		Employee emp = (Employee)obj;
		return emp.getId() == getId() && emp.getName().equals(getName());
	}

	public String toString() {
		return String.format("Employee(%d, %s)", getId(), getName());
	}
}

