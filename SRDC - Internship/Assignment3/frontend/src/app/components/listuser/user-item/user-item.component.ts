import {Component, Output} from '@angular/core';
import {User} from "../../../../User";
import {Input, EventEmitter} from "@angular/core";
import {faTrash, faCog} from "@fortawesome/free-solid-svg-icons";
import {ApiService} from "../../../services/api.service";

@Component({
  selector: 'app-user-item',
  templateUrl: './user-item.component.html',
  styleUrls: ['./user-item.component.css']
})
export class UserItemComponent {
  username: string;
  name: string;
  admin: string;
  @Input() user: User;
  @Input() showInfo: boolean;
  @Input() showFields: boolean[];
  @Input() head: string = "username";
  infoRequired: boolean = false;
  updateRequired: boolean = false;
  faTrash = faTrash;
  faCog = faCog;
  @Output() deleteClick: EventEmitter<any> = new EventEmitter<any>();

  constructor(private loginService: ApiService) {
  }
  onInfo(){
    this.infoRequired = !this.infoRequired;
  }

  onDelete(){
    if (confirm('Are you sure to delete '+ this.user.username)) {
      this.deleteClick.emit();
    } else {
      // Do nothing!
      return;
    }
  }

  onUpdate(){
    if(this.updateRequired){
      this.updateRequired = false;
      return;
    }
    if (confirm('Do you want to update '+ this.user.username)) {
      this.updateRequired = true;
    } else {
      // Do nothing!
      return;
    }
  }

  changeUser(user){
    this.user = user;
    this.updateRequired = false;
  }

}
